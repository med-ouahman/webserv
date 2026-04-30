#!/usr/bin/env python3
"""
test_parser.py — Python HTTP Parser Test Client
Connects via raw TCP sockets. No http.client, no urllib, no requests.
Usage: python3 test_parser.py [host] [port]
"""

import socket
import sys
import time

HOST = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8080

PASS = 0
FAIL = 0
CRLF = b"\r\n"


# ── helpers ────────────────────────────────────────────────────────────────────

def connect():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5)
    s.connect((HOST, PORT))
    return s


def recv_all(s, timeout=2):
    """Read until the socket is quiet for `timeout` seconds."""
    s.settimeout(timeout)
    data = b""
    try:
        while True:
            chunk = s.recv(4096)
            if not chunk:
                break
            data += chunk
    except socket.timeout:
        pass
    return data


def parse_response(raw):
    """Returns (status_code, headers_dict, body_bytes)."""
    if b"\r\n\r\n" not in raw:
        return None, {}, b""
    header_part, body = raw.split(b"\r\n\r\n", 1)
    lines = header_part.split(b"\r\n")
    status_line = lines[0].decode(errors="replace")
    try:
        code = int(status_line.split()[1])
    except (IndexError, ValueError):
        code = 0
    headers = {}
    for line in lines[1:]:
        if b":" in line:
            k, v = line.split(b":", 1)
            headers[k.strip().lower().decode()] = v.strip().decode()
    return code, headers, body


def check(label, condition, detail=""):
    global PASS, FAIL
    if condition:
        PASS += 1
        print(f"  ✓  {label}")
    else:
        FAIL += 1
        print(f"  ✗  FAIL: {label}" + (f" ({detail})" if detail else ""))


def suite(name):
    print(f"\n── {name} ──")


# ── test cases ─────────────────────────────────────────────────────────────────

def test_simple_get():
    suite("Simple GET")
    s = connect()
    req = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    s.sendall(req)
    raw = recv_all(s)
    s.close()
    code, headers, body = parse_response(raw)
    check("GET / returns 2xx or 3xx", 200 <= code < 400, f"got {code}")
    check("Response has Content-Length or Transfer-Encoding",
          "content-length" in headers or "transfer-encoding" in headers)


def test_post_with_body():
    suite("POST with body")
    body = b"hello=world&foo=bar"
    s = connect()
    req = (
        b"POST /echo HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"Content-Type: application/x-www-form-urlencoded\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"\r\n" + body
    )
    s.sendall(req)
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("POST returns a valid HTTP response", code > 0, f"got {code}")
    check("POST does not return 5xx", code < 500, f"got {code}")


def test_content_length_zero():
    suite("Content-Length: 0 (no body hang)")
    s = connect()
    req = b"POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n"
    s.sendall(req)
    # Server must respond — if it hangs here the test will timeout
    raw = recv_all(s, timeout=3)
    s.close()
    code, _, _ = parse_response(raw)
    check("CL:0 — server responds (no hang)", len(raw) > 0,
          "got no response — parser likely stuck in NEED_MORE")
    check("CL:0 — valid status code", code > 0, f"got {code}")


def test_leading_crlf_telnet():
    suite("Leading CRLF before request (telnet artifact)")
    s = connect()
    # Prepend \r\n as telnet would leave after a previous body
    req = b"\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n"
    s.sendall(req)
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("Leading CRLF — server responds", len(raw) > 0)
    check("Leading CRLF — not 400", code != 400, f"got {code}")


def test_keep_alive():
    suite("Keep-alive: 5 sequential requests on one connection")
    s = connect()
    results = []
    for i in range(5):
        uri = f"/keepalive/{i}".encode()
        req = b"GET " + uri + b" HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        s.sendall(req)
        time.sleep(0.05)
    raw = recv_all(s)
    s.close()
    # Count HTTP/1.1 or HTTP/1.0 response lines
    count = raw.count(b"HTTP/1.")
    check("All 5 responses received", count == 5, f"got {count} responses")


def test_chunked_delivery():
    suite("Chunked delivery (parser accumulation)")
    s = connect()
    req = b"GET /chunked HTTP/1.1\r\nHost: localhost\r\n\r\n"
    # Send 2 bytes at a time with a tiny pause
    for i in range(0, len(req), 2):
        s.send(req[i:i+2])
        time.sleep(0.005)
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("Chunked delivery — server responds", len(raw) > 0)
    check("Chunked delivery — valid status", code > 0, f"got {code}")


def test_bad_request():
    suite("Malformed request → 400")
    s = connect()
    s.sendall(b"NOT VALID HTTP AT ALL\r\n\r\n")
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("Bad request returns 400", code == 400, f"got {code}")


def test_missing_host_http11():
    suite("Missing Host header (HTTP/1.1) → 400")
    s = connect()
    s.sendall(b"GET / HTTP/1.1\r\n\r\n")
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("Missing Host → 400", code == 400, f"got {code}")


def test_large_body():
    suite("Large body (64 KiB)")
    body = b"X" * 65536
    s = connect()
    req = (
        b"POST /large HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"\r\n" + body
    )
    s.sendall(req)
    raw = recv_all(s, timeout=5)
    s.close()
    code, _, _ = parse_response(raw)
    check("Large body — server responds", len(raw) > 0)
    check("Large body — not 5xx", 0 < code < 500, f"got {code}")


def test_head_method():
    suite("HEAD method — no body in response")
    s = connect()
    s.sendall(b"HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n")
    raw = recv_all(s)
    s.close()
    code, _, body = parse_response(raw)
    check("HEAD returns 2xx", 200 <= code < 300, f"got {code}")
    check("HEAD response has no body", len(body) == 0, f"got {len(body)} body bytes")


def test_http10():
    suite("HTTP/1.0 (no Host required by parser)")
    s = connect()
    s.sendall(b"GET / HTTP/1.0\r\n\r\n")
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("HTTP/1.0 GET — server responds", len(raw) > 0)
    check("HTTP/1.0 GET — valid code", code > 0, f"got {code}")


def test_delete_method():
    suite("DELETE method")
    s = connect()
    s.sendall(b"DELETE /resource/42 HTTP/1.1\r\nHost: localhost\r\n\r\n")
    raw = recv_all(s)
    s.close()
    code, _, _ = parse_response(raw)
    check("DELETE returns a valid HTTP response", code > 0, f"got {code}")


def test_pipelined():
    suite("Pipelined requests (two requests in one send)")
    s = connect()
    req1 = b"GET /pipeline/1 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    req2 = b"GET /pipeline/2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    s.sendall(req1 + req2)
    raw = recv_all(s)
    s.close()
    count = raw.count(b"HTTP/1.")
    check("Both pipelined responses received", count == 2, f"got {count}")


# ── main ───────────────────────────────────────────────────────────────────────

def main():
    print(f"Python HTTP Parser Test Client → {HOST}:{PORT}")
    print("=" * 50)

    try:
        connect().close()
    except ConnectionRefusedError:
        print(f"ERROR: Cannot connect to {HOST}:{PORT} — is the server running?")
        sys.exit(1)

    test_simple_get()
    test_post_with_body()
    test_content_length_zero()
    test_leading_crlf_telnet()
    test_keep_alive()
    test_chunked_delivery()
    test_bad_request()
    test_missing_host_http11()
    test_large_body()
    test_head_method()
    test_http10()
    test_delete_method()
    test_pipelined()

    print(f"\n{'=' * 50}")
    total = PASS + FAIL
    print(f"  Python results: {PASS}/{total} passed", end="")
    print(f"  ({FAIL} failed)" if FAIL else "")
    print("=" * 50)
    sys.exit(0 if FAIL == 0 else 1)


if __name__ == "__main__":
    main()
