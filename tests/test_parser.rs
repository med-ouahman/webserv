// test_parser.rs — Rust HTTP Parser Test Client
// Pure std::net::TcpStream. No reqwest, no hyper, no tokio.
//
// Build:  rustc -o test_parser_rs test_parser.rs
//   OR:   cargo run --bin test_parser -- [host] [port]
// Usage:  ./test_parser_rs [host] [port]

use std::io::{Read, Write};
use std::net::TcpStream;
use std::time::Duration;
use std::env;

// ── state ─────────────────────────────────────────────────────────────────────

struct Stats {
    pass:  u32,
    fail:  u32,
    total: u32,
}

impl Stats {
    fn new() -> Self { Stats { pass: 0, fail: 0, total: 0 } }

    fn check(&mut self, label: &str, condition: bool, detail: &str) {
        self.total += 1;
        if condition {
            self.pass += 1;
            println!("  ✓  {}", label);
        } else {
            self.fail += 1;
            if detail.is_empty() {
                println!("  ✗  FAIL: {}", label);
            } else {
                println!("  ✗  FAIL: {} ({})", label, &detail[..detail.len().min(80)]);
            }
        }
    }
}

// ── helpers ───────────────────────────────────────────────────────────────────

fn suite(name: &str) {
    println!("\n── {} ──", name);
}

fn connect(host: &str, port: u16) -> Option<TcpStream> {
    let addr = format!("{}:{}", host, port);
    match TcpStream::connect(&addr) {
        Ok(s) => {
            s.set_read_timeout(Some(Duration::from_secs(3))).ok();
            s.set_write_timeout(Some(Duration::from_secs(3))).ok();
            Some(s)
        }
        Err(e) => {
            eprintln!("  connect error: {}", e);
            None
        }
    }
}

fn send_all(stream: &mut TcpStream, data: &[u8]) -> bool {
    stream.write_all(data).is_ok()
}

fn recv_all(stream: &mut TcpStream) -> Vec<u8> {
    let mut buf  = Vec::new();
    let mut tmp  = [0u8; 4096];
    loop {
        match stream.read(&mut tmp) {
            Ok(0) => break,
            Ok(n) => buf.extend_from_slice(&tmp[..n]),
            Err(_) => break,
        }
    }
    buf
}

fn parse_status(raw: &[u8]) -> u16 {
    // "HTTP/1.x NNN ..."
    let s = std::str::from_utf8(raw).unwrap_or("");
    let sp = s.find(' ').unwrap_or(0);
    if sp == 0 { return 0; }
    s[sp+1..].splitn(2, ' ').next()
        .and_then(|t| t.trim().parse().ok())
        .unwrap_or(0)
}

fn count_occurrences(haystack: &[u8], needle: &[u8]) -> usize {
    let mut count = 0;
    let mut pos   = 0;
    while pos + needle.len() <= haystack.len() {
        if &haystack[pos..pos+needle.len()] == needle {
            count += 1;
            pos   += needle.len();
        } else {
            pos += 1;
        }
    }
    count
}

// ── test cases ────────────────────────────────────────────────────────────────

fn test_simple_get(st: &mut Stats, host: &str, port: u16) {
    suite("Simple GET");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("GET / returns 2xx or 3xx", code >= 200 && code < 400, &code.to_string());
    st.check("Response non-empty",       !resp.is_empty(), "");
}

fn test_post_with_body(st: &mut Stats, host: &str, port: u16) {
    suite("POST with body");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    let body = b"rust_field=rust_value&version=2021";
    let req  = format!(
        "POST /echo HTTP/1.1\r\nHost: localhost\r\n\
         Content-Type: application/x-www-form-urlencoded\r\n\
         Content-Length: {}\r\n\r\n",
        body.len()
    );
    send_all(&mut s, req.as_bytes());
    send_all(&mut s, body);
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("POST — valid response code", code > 0,   &code.to_string());
    st.check("POST — not 5xx",             code < 500, &code.to_string());
}

fn test_content_length_zero(st: &mut Stats, host: &str, port: u16) {
    suite("Content-Length: 0 (no body hang)");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");
    let resp = recv_all(&mut s);
    st.check(
        "CL:0 — server responds (no hang)",
        !resp.is_empty(),
        "got no response — parser likely stuck in NEED_MORE",
    );
    st.check("CL:0 — valid status code", parse_status(&resp) > 0, "");
}

fn test_leading_crlf(st: &mut Stats, host: &str, port: u16) {
    suite("Leading CRLF before request (telnet artifact)");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n");
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("Leading CRLF — server responds", !resp.is_empty(), "");
    st.check("Leading CRLF — not 400",         code != 400, &code.to_string());
}

fn test_bad_request(st: &mut Stats, host: &str, port: u16) {
    suite("Malformed request → 400");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"GARBAGE NOT HTTP AT ALL\r\n\r\n");
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("Bad request — returns 400", code == 400, &code.to_string());
}

fn test_missing_host(st: &mut Stats, host: &str, port: u16) {
    suite("Missing Host header (HTTP/1.1) → 400");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"GET / HTTP/1.1\r\n\r\n");
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("Missing Host → 400", code == 400, &code.to_string());
}

fn test_byte_by_byte(st: &mut Stats, host: &str, port: u16) {
    suite("Byte-by-byte delivery (parser accumulation)");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    let req = b"GET /bytewise HTTP/1.1\r\nHost: localhost\r\n\r\n";
    for byte in req.iter() {
        if s.write_all(&[*byte]).is_err() { break; }
        std::thread::sleep(Duration::from_micros(500));
    }
    let resp = recv_all(&mut s);
    st.check("Byte-by-byte — server responds",   !resp.is_empty(), "");
    st.check("Byte-by-byte — valid status code", parse_status(&resp) > 0, "");
}

fn test_large_body(st: &mut Stats, host: &str, port: u16) {
    suite("Large body (64 KiB)");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    let body = vec![b'R'; 65536];
    let header = format!(
        "POST /large HTTP/1.1\r\nHost: localhost\r\nContent-Length: {}\r\n\r\n",
        body.len()
    );
    send_all(&mut s, header.as_bytes());
    send_all(&mut s, &body);
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check("Large body — server responds", !resp.is_empty(), "");
    st.check("Large body — not 5xx",         code > 0 && code < 500, &code.to_string());
}

fn test_pipelined(st: &mut Stats, host: &str, port: u16) {
    suite("Pipelined requests");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s,
        b"GET /pipe/rs/1 HTTP/1.1\r\nHost: localhost\r\n\r\n\
          GET /pipe/rs/2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    );
    let resp  = recv_all(&mut s);
    let count = count_occurrences(&resp, b"HTTP/1.");
    st.check("Both pipelined responses received", count == 2, &format!("got {}", count));
}

fn test_http10(st: &mut Stats, host: &str, port: u16) {
    suite("HTTP/1.0");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"GET / HTTP/1.0\r\n\r\n");
    let resp = recv_all(&mut s);
    st.check("HTTP/1.0 — server responds",   !resp.is_empty(), "");
    st.check("HTTP/1.0 — valid status code", parse_status(&resp) > 0, "");
}

fn test_keep_alive(st: &mut Stats, host: &str, port: u16) {
    suite("Keep-alive: 5 requests on one connection");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    for i in 1..=5u32 {
        let conn = if i < 5 { "keep-alive" } else { "close" };
        let req  = format!(
            "GET /ka/rs/{} HTTP/1.1\r\nHost: localhost\r\nConnection: {}\r\n\r\n",
            i, conn
        );
        if send_all(&mut s, req.as_bytes()) { } else { break; }
    }
    let resp  = recv_all(&mut s);
    let count = count_occurrences(&resp, b"HTTP/1.");
    st.check("All 5 keep-alive responses received", count == 5, &format!("got {}", count));
}

fn test_header_case_variants(st: &mut Stats, host: &str, port: u16) {
    suite("Content-Length case variants");
    let variants = [
        "content-length", "Content-Length", "CONTENT-LENGTH", "cOnTeNt-LeNgTh",
    ];
    for v in &variants {
        let mut s = match connect(host, port) {
            Some(s) => s,
            None    => { st.check(&format!("connect ({})", v), false, ""); continue; }
        };
        let body = "hi";
        let req  = format!(
            "POST /case HTTP/1.1\r\nHost: localhost\r\n{}: {}\r\n\r\n{}",
            v, body.len(), body
        );
        send_all(&mut s, req.as_bytes());
        let resp = recv_all(&mut s);
        let code = parse_status(&resp);
        st.check(
            &format!("CL variant '{}' — not 400", v),
            code != 400,
            &code.to_string(),
        );
    }
}

fn test_non_ascii_garbage(st: &mut Stats, host: &str, port: u16) {
    suite("Non-ASCII / binary garbage → 400");
    let mut s = match connect(host, port) {
        Some(s) => s,
        None    => { st.check("connect", false, ""); return; }
    };
    send_all(&mut s, b"\xff\xfe\xfd INVALID HTTP\r\n\r\n");
    let resp = recv_all(&mut s);
    let code = parse_status(&resp);
    st.check(
        "Binary garbage — returns 400 or closes",
        code == 400 || resp.is_empty(),
        &code.to_string(),
    );
}

// ── main ──────────────────────────────────────────────────────────────────────

fn main() {
    let args: Vec<String> = env::args().collect();
    let host = args.get(1).map(|s| s.as_str()).unwrap_or("127.0.0.1");
    let port: u16 = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(8080);

    println!("Rust HTTP Parser Test Client → {}:{}", host, port);
    println!("{}", "=".repeat(50));

    // Connectivity check
    if connect(host, port).is_none() {
        eprintln!("ERROR: Cannot connect to {}:{} — is the server running?", host, port);
        std::process::exit(1);
    }

    let mut st = Stats::new();

    test_simple_get          (&mut st, host, port);
    test_post_with_body      (&mut st, host, port);
    test_content_length_zero (&mut st, host, port);
    test_leading_crlf        (&mut st, host, port);
    test_bad_request         (&mut st, host, port);
    test_missing_host        (&mut st, host, port);
    test_byte_by_byte        (&mut st, host, port);
    test_large_body          (&mut st, host, port);
    test_pipelined           (&mut st, host, port);
    test_http10              (&mut st, host, port);
    test_keep_alive          (&mut st, host, port);
    test_header_case_variants(&mut st, host, port);
    test_non_ascii_garbage   (&mut st, host, port);

    println!("\n{}", "=".repeat(50));
    print!("  Rust results: {}/{} passed", st.pass, st.total);
    if st.fail > 0 { print!("  ({} failed)", st.fail); }
    println!("\n{}", "=".repeat(50));

    std::process::exit(if st.fail > 0 { 1 } else { 0 });
}
