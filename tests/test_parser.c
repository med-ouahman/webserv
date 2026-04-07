/*
 * test_parser.c — C HTTP Parser Test Client
 * Raw POSIX sockets only. No libcurl, no http libraries.
 *
 * Build:  gcc -std=c11 -Wall -Wextra -o test_parser_c test_parser.c
 * Usage:  ./test_parser_c [host] [port]
 */

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE          /* usleep on glibc */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

/* ── configuration ─────────────────────────────────────────────────────────── */

static const char *g_host = "127.0.0.1";
static int         g_port = 8080;

/* ── counters ──────────────────────────────────────────────────────────────── */

static int g_pass  = 0;
static int g_fail  = 0;
static int g_total = 0;

/* ── helpers ───────────────────────────────────────────────────────────────── */

static void suite(const char *name)
{
    printf("\n── %s ──\n", name);
}

static void check(const char *label, int condition, const char *detail)
{
    ++g_total;
    if (condition) {
        ++g_pass;
        printf("  ✓  %s\n", label);
    } else {
        ++g_fail;
        if (detail && *detail)
            printf("  ✗  FAIL: %s (%s)\n", label, detail);
        else
            printf("  ✗  FAIL: %s\n", label);
    }
}

/* Open a non-blocking-capable TCP socket connected to g_host:g_port.
   Returns -1 on failure. */
static int tcp_connect(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    /* 3-second recv timeout */
    struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons((uint16_t)g_port);
    addr.sin_addr.s_addr = inet_addr(g_host);

    if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }
    return fd;
}

/* Receive all available bytes until timeout or EOF.
   Returns bytes written into buf; buf is always NUL-terminated. */
static ssize_t recv_all(int fd, char *buf, size_t cap)
{
    size_t total = 0;
    ssize_t n;
    while (total < cap - 1) {
        n = recv(fd, buf + total, cap - 1 - total, 0);
        if (n <= 0) break;
        total += (size_t)n;
    }
    buf[total] = '\0';
    return (ssize_t)total;
}

/* Parse the HTTP status code from a raw response.
   Returns 0 if not parseable. */
static int parse_status(const char *raw)
{
    /* "HTTP/1.x NNN ..." */
    const char *sp = strchr(raw, ' ');
    if (!sp) return 0;
    return atoi(sp + 1);
}

/* Count occurrences of needle in haystack. */
static int count_substr(const char *haystack, const char *needle)
{
    int count = 0;
    const char *p = haystack;
    size_t nlen = strlen(needle);
    while ((p = strstr(p, needle)) != NULL) {
        ++count;
        p += nlen;
    }
    return count;
}

/* Send all bytes in buf. Returns 0 on success, -1 on error. */
static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += (size_t)n;
    }
    return 0;
}

/* ── test cases ────────────────────────────────────────────────────────────── */

#define BUF 131072   /* 128 KiB response buffer */

static void test_simple_get(void)
{
    suite("Simple GET");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send_all(fd, req, strlen(req));

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    int code = parse_status(buf);
    check("GET / returns 2xx or 3xx", code >= 200 && code < 400, buf);
    check("Response non-empty",        n > 0, "");
}

static void test_post_with_body(void)
{
    suite("POST with body");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *body = "field=value&x=1";
    char req[512];
    snprintf(req, sizeof req,
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %zu\r\n"
        "\r\n%s",
        strlen(body), body);

    send_all(fd, req, strlen(req));

    char buf[BUF];
    recv_all(fd, buf, sizeof buf);
    close(fd);

    int code = parse_status(buf);
    check("POST — valid response code", code > 0, buf);
    check("POST — not 5xx",             code < 500, buf);
}

static void test_content_length_zero(void)
{
    suite("Content-Length: 0 (no body hang)");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *req = "POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n";
    send_all(fd, req, strlen(req));

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    check("CL:0 — server responds (no hang)", n > 0,
          "got no response — parser likely stuck in NEED_MORE");
    check("CL:0 — valid status code", parse_status(buf) > 0, buf);
}

static void test_leading_crlf(void)
{
    suite("Leading CRLF before request (telnet artifact)");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *req = "\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send_all(fd, req, strlen(req));

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    int code = parse_status(buf);
    check("Leading CRLF — server responds",  n > 0, "");
    check("Leading CRLF — not 400",          code != 400, buf);
}

static void test_bad_request(void)
{
    suite("Malformed request → 400");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    send_all(fd, "GARBAGE NOT HTTP AT ALL\r\n\r\n", 26);

    char buf[BUF];
    recv_all(fd, buf, sizeof buf);
    close(fd);

    check("Bad request — returns 400", parse_status(buf) == 400, buf);
}

static void test_missing_host(void)
{
    suite("Missing Host header (HTTP/1.1) → 400");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    send_all(fd, "GET / HTTP/1.1\r\n\r\n", 18);

    char buf[BUF];
    recv_all(fd, buf, sizeof buf);
    close(fd);

    check("Missing Host → 400", parse_status(buf) == 400, buf);
}

static void test_chunked_delivery(void)
{
    suite("Chunked delivery (4 bytes at a time)");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *req = "GET /chunked HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len = strlen(req);
    for (size_t i = 0; i < len; i += 4) {
        size_t chunk = (i + 4 <= len) ? 4 : len - i;
        send(fd, req + i, chunk, 0);
        usleep(2000);   /* 2 ms gap */
    }

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    check("Chunked delivery — server responds",   n > 0, "");
    check("Chunked delivery — valid status code", parse_status(buf) > 0, buf);
}

static void test_large_body(void)
{
    suite("Large body (64 KiB)");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const size_t body_len = 65536;
    char *body = malloc(body_len);
    if (!body) { check("malloc", 0, "OOM"); close(fd); return; }
    memset(body, 'X', body_len);

    char header[256];
    snprintf(header, sizeof header,
        "POST /large HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        body_len);

    send_all(fd, header, strlen(header));
    send_all(fd, body, body_len);
    free(body);

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    int code = parse_status(buf);
    check("Large body — server responds", n > 0, "");
    check("Large body — not 5xx",         0 < code && code < 500, buf);
}

static void test_pipelined(void)
{
    suite("Pipelined requests");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *both =
        "GET /pipe/1 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        "GET /pipe/2 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send_all(fd, both, strlen(both));

    char buf[BUF];
    recv_all(fd, buf, sizeof buf);
    close(fd);

    int count = count_substr(buf, "HTTP/1.");
    check("Both pipelined responses received", count == 2, buf);
}

static void test_http10(void)
{
    suite("HTTP/1.0 (no Host required by parser)");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    send_all(fd, "GET / HTTP/1.0\r\n\r\n", 18);

    char buf[BUF];
    ssize_t n = recv_all(fd, buf, sizeof buf);
    close(fd);

    check("HTTP/1.0 — server responds",   n > 0, "");
    check("HTTP/1.0 — valid status code", parse_status(buf) > 0, buf);
}

static void test_keep_alive(void)
{
    suite("Keep-alive: 3 requests on one connection");
    int fd = tcp_connect(); if (fd < 0) { check("connect", 0, ""); return; }

    const char *req =
        "GET /ka/1 HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        "GET /ka/2 HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        "GET /ka/3 HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send_all(fd, req, strlen(req));

    char buf[BUF];
    recv_all(fd, buf, sizeof buf);
    close(fd);

    int count = count_substr(buf, "HTTP/1.");
    check("All 3 keep-alive responses received", count == 3, buf);
}

/* ── main ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
    if (argc > 1) g_host = argv[1];
    if (argc > 2) g_port = atoi(argv[2]);

    printf("C HTTP Parser Test Client → %s:%d\n", g_host, g_port);
    printf("==================================================\n");

    /* Quick connectivity check */
    int fd = tcp_connect();
    if (fd < 0) {
        fprintf(stderr, "ERROR: Cannot connect to %s:%d — is the server running?\n",
                g_host, g_port);
        return 1;
    }
    close(fd);

    test_simple_get();
    test_post_with_body();
    test_content_length_zero();
    test_leading_crlf();
    test_bad_request();
    test_missing_host();
    test_chunked_delivery();
    test_large_body();
    test_pipelined();
    test_http10();
    test_keep_alive();

    printf("\n==================================================\n");
    printf("  C results: %d/%d passed", g_pass, g_total);
    if (g_fail) printf("  (%d failed)", g_fail);
    printf("\n==================================================\n");

    return g_fail > 0 ? 1 : 0;
}
