/*
 * test_parser.cpp — C++ HTTP Parser Test Client
 * Raw TCP sockets with RAII wrappers. No libcurl, no Boost.Asio.
 *
 * Build:  g++ -std=c++17 -Wall -Wextra -o test_parser_cpp test_parser.cpp
 * Usage:  ./test_parser_cpp [host] [port]
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// ── configuration ─────────────────────────────────────────────────────────────

static std::string g_host = "127.0.0.1";
static int         g_port = 8080;

// ── counters ──────────────────────────────────────────────────────────────────

static int g_pass  = 0;
static int g_fail  = 0;
static int g_total = 0;

// ── RAII socket ───────────────────────────────────────────────────────────────

struct TCPSocket
{
    int fd = -1;

    explicit TCPSocket()
    {
        fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) return;

        struct timeval tv{ .tv_sec = 3, .tv_usec = 0 };
        ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

        struct sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(static_cast<uint16_t>(g_port));
        addr.sin_addr.s_addr = ::inet_addr(g_host.c_str());

        if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) {
            ::close(fd);
            fd = -1;
        }
    }

    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    bool good() const { return fd >= 0; }

    bool send_all(const std::string& data)
    {
        size_t sent = 0;
        while (sent < data.size()) {
            auto n = ::send(fd, data.data() + sent, data.size() - sent, 0);
            if (n <= 0) return false;
            sent += static_cast<size_t>(n);
        }
        return true;
    }

    std::string recv_all()
    {
        std::string result;
        char buf[4096];
        ssize_t n;
        while ((n = ::recv(fd, buf, sizeof buf, 0)) > 0)
            result.append(buf, static_cast<size_t>(n));
        if (n < 0) {
            std::cerr << "ERROR: " << strerror(errno) << "\n";
        }
        return result;
    }

    // Non-copyable
    TCPSocket(const TCPSocket&)            = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
};

// ── helpers ───────────────────────────────────────────────────────────────────

void suite(const std::string& name)
{
    std::cout << "\n── " << name << " ──\n";
}

void check(const std::string& label, bool condition, const std::string& detail = "")
{
    ++g_total;
    if (condition) {
        ++g_pass;
        std::cout << "  ✓  " << label << "\n";
    } else {
        ++g_fail;
        std::cout << "  ✗  FAIL: " << label;
        if (!detail.empty()) std::cout << " (" << detail.substr(0, 80) << ")";
        std::cout << "\n";
    }
}

int parse_status(const std::string& raw)
{
    auto sp = raw.find(' ');
    if (sp == std::string::npos) return 0;
    try { return std::stoi(raw.substr(sp + 1, 3)); }
    catch (...) { return 0; }
}

int count_occurrences(const std::string& haystack, const std::string& needle)
{
    int count = 0;
    size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

// ── test cases ────────────────────────────────────────────────────────────────

void test_simple_get()
{
    suite("Simple GET");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    auto resp = s.recv_all();
    int  code = parse_status(resp);

    check("GET / returns 2xx or 3xx", code >= 200 && code < 400, std::to_string(code));
    check("Response non-empty",       !resp.empty());
}

void test_post_with_body()
{
    suite("POST with body");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    std::string body = "cpp_field=cpp_value&x=42";
    std::string req  =
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;

    s.send_all(req);
    auto resp = s.recv_all();
    int  code = parse_status(resp);

    check("POST — valid response code", code > 0,   std::to_string(code));
    check("POST — not 5xx",             code < 500, std::to_string(code));
}

void test_content_length_zero()
{
    suite("Content-Length: 0 (no body hang)");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");
    auto resp = s.recv_all();

    check("CL:0 — server responds (no hang)",
          !resp.empty(), "got no response — parser likely stuck in NEED_MORE");
    check("CL:0 — valid status code", parse_status(resp) > 0);
}

void test_leading_crlf()
{
    suite("Leading CRLF before request (telnet artifact)");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n");
    auto resp = s.recv_all();
    int  code = parse_status(resp);

    check("Leading CRLF — server responds", !resp.empty());
    check("Leading CRLF — not 400",         code != 400, std::to_string(code));
}

void test_bad_request()
{
    suite("Malformed request → 400");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("THIS IS NOT HTTP AT ALL\r\n\r\n");
    auto resp = s.recv_all();

    check("Bad request — returns 400", parse_status(resp) == 400, resp.substr(0, 30));
}

void test_missing_host()
{
    suite("Missing Host header (HTTP/1.1) → 400");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("GET / HTTP/1.1\r\n\r\n");
    auto resp = s.recv_all();

    check("Missing Host → 400", parse_status(resp) == 400, resp.substr(0, 30));
}

void test_byte_by_byte()
{
    suite("Byte-by-byte delivery (parser accumulation)");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    std::string req = "GET /bytewise HTTP/1.1\r\nHost: localhost\r\n\r\n";
    for (char c : req) {
        char b[1] = { c };
        ::send(s.fd, b, 1, 0);
        ::usleep(500);   /* 0.5 ms per byte */
    }

    auto resp = s.recv_all();
    check("Byte-by-byte — server responds",   !resp.empty());
    check("Byte-by-byte — valid status code", parse_status(resp) > 0);
}

void test_large_body()
{
    suite("Large body (64 KiB)");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    std::string body(65536, 'C');
    std::string req =
        "POST /large HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;

    s.send_all(req);
    auto resp = s.recv_all();
    int  code = parse_status(resp);

    check("Large body — server responds", !resp.empty());
    check("Large body — not 5xx",         0 < code && code < 500, std::to_string(code));
}

void test_pipelined()
{
    suite("Pipelined requests");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all(
        "GET /pipe/cpp/1 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        "GET /pipe/cpp/2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    );
    auto resp  = s.recv_all();
    int  count = count_occurrences(resp, "HTTP/1.");

    check("Both pipelined responses received", count == 2, "got " + std::to_string(count));
}

void test_http10()
{
    suite("HTTP/1.0");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    s.send_all("GET / HTTP/1.0\r\n\r\n");
    auto resp = s.recv_all();

    check("HTTP/1.0 — server responds",   !resp.empty());
    check("HTTP/1.0 — valid status code", parse_status(resp) > 0);
}

void test_keep_alive( int n )
{
    suite("Keep-alive: 4 requests on one connection");
    TCPSocket s;
    if (!s.good()) { check("connect", false); return; }

    for (int i = 1; i <= n; ++i) {
        std::string conn = (i < 4) ? "keep-alive" : "close";
        s.send_all("GET /ka/cpp/" + std::to_string(i) + " HTTP/1.1\r\n"
                   "Host: localhost\r\n"
                   "Connection: " + conn + "\r\n\r\n");
    }

    auto resp  = s.recv_all();
    std::cout << resp << "\n";
    int  count = count_occurrences(resp, "HTTP/1.");
    check("All 4 keep-alive responses received", count == n, "got " + std::to_string(count));
    
}

void test_header_case()
{
    suite("Content-Length case variants");
    for (const auto& variant : {
            "content-length", "Content-Length",
            "CONTENT-LENGTH", "cOnTeNt-LeNgTh"})
    {
        TCPSocket s;
        if (!s.good()) { check("connect", false); return; }

        std::string body = "hello";
        std::string req  =
            std::string("POST /case HTTP/1.1\r\nHost: localhost\r\n") +
            variant + ": " + std::to_string(body.size()) + "\r\n\r\n" + body;

        s.send_all(req);
        auto resp = s.recv_all();
        int  code = parse_status(resp);

        check(std::string("CL variant '") + variant + "' — not 400",
              code != 400, std::to_string(code));
    }
}

void test_delete_put()
{
    suite("DELETE and PUT methods");
    {
        TCPSocket s;
        if (s.good()) {
            s.send_all("DELETE /resource/1 HTTP/1.1\r\nHost: localhost\r\n\r\n");
            auto resp = s.recv_all();
            check("DELETE — valid response", parse_status(resp) > 0);
        }
    }
    {
        TCPSocket s;
        if (s.good()) {
            std::string body = "updated content";
            std::string req  =
                "PUT /resource/1 HTTP/1.1\r\nHost: localhost\r\n"
                "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
            s.send_all(req);
            auto resp = s.recv_all();
            check("PUT — valid response", parse_status(resp) > 0);
        }
    }
}

void test_slow_client() {
    TCPSocket s;
    if (!s.good()) {
        return ;
    }
    size_t i;
    std::string req = "GET / HTTP/1.1\r\n";
    while (true) {
        
        std::string a;
        if (i < req.length()) {

            i++;
            
            a = &req[i];
        } else {
            a = "g";
        }
        s.send_all(a);
        sleep(1);
    }
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    if (argc > 1) g_host = argv[1];
    if (argc > 2) g_port = std::stoi(argv[2]);

    std::cout << "C++ HTTP Parser Test Client → " << g_host << ":" << g_port << "\n";
    std::cout << std::string(50, '=') << "\n";
        
    // test_slow_client();
    test_keep_alive(4);
    // test_bad_request();
    // test_byte_by_byte();
    // test_content_length_zero();
    // test_delete_put();
    // test_header_case();
    // test_http10();
    // test_keep_alive(4);
    // test_large_body();
    // test_missing_host();
    // test_pipelined();
    // test_post_with_body();
    // test_simple_get();    
    // test_leading_crlf();
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "  C++ results: " << g_pass << "/" << g_total << " passed";
    if (g_fail) std::cout << "  (" << g_fail << " failed)";
    std::cout << "\n" << std::string(50, '=') << "\n";

    return !!g_fail;
}
