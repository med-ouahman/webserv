<?php
/**
 * test_parser.php — PHP HTTP Parser Test Client
 * Uses fsockopen for raw TCP. No cURL, no file_get_contents HTTP wrapper.
 *
 * Usage: php test_parser.php [host] [port]
 */

declare(strict_types=1);

$host = $argv[1] ?? '127.0.0.1';
$port = (int)($argv[2] ?? 8080);

$pass  = 0;
$fail  = 0;
$total = 0;

// ── helpers ───────────────────────────────────────────────────────────────────

function suite(string $name): void {
    echo "\n── {$name} ──\n";
}

function check(string $label, bool $condition, string $detail = ''): void {
    global $pass, $fail, $total;
    ++$total;
    if ($condition) {
        ++$pass;
        echo "  ✓  {$label}\n";
    } else {
        ++$fail;
        $d = $detail ? " ({$detail})" : '';
        echo "  ✗  FAIL: {$label}{$d}\n";
    }
}

/**
 * Open a TCP connection. Returns resource|false.
 * Timeout = 3 s for connect; 3 s for recv.
 */
function tcp_connect(string $host, int $port) {
    $errno  = 0;
    $errstr = '';
    $sock   = @fsockopen($host, $port, $errno, $errstr, 3.0);
    if ($sock === false) return false;
    stream_set_timeout($sock, 3);
    return $sock;
}

/** Read until timeout or EOF. */
function recv_all($sock): string {
    $data = '';
    while (!feof($sock)) {
        $chunk = fread($sock, 4096);
        if ($chunk === false || $chunk === '') {
            $info = stream_get_meta_data($sock);
            if ($info['timed_out']) break;
            break;
        }
        $data .= $chunk;
    }
    return $data;
}

/** Parse status code from raw HTTP response. */
function parse_status(string $raw): int {
    if (preg_match('/^HTTP\/\d\.\d (\d{3})/', $raw, $m)) {
        return (int)$m[1];
    }
    return 0;
}

/** Count occurrences of needle in haystack. */
function count_substr(string $haystack, string $needle): int {
    return substr_count($haystack, $needle);
}

// ── test cases ────────────────────────────────────────────────────────────────

function test_simple_get(string $host, int $port): void {
    suite('Simple GET');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    $code = parse_status($resp);
    check('GET / returns 2xx or 3xx',  $code >= 200 && $code < 400, (string)$code);
    check('Response non-empty',         strlen($resp) > 0);
}

function test_post_with_body(string $host, int $port): void {
    suite('POST with body');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    $body = 'php_field=php_value&lang=php';
    $req  = "POST /echo HTTP/1.1\r\n"
          . "Host: localhost\r\n"
          . "Content-Type: application/x-www-form-urlencoded\r\n"
          . "Content-Length: " . strlen($body) . "\r\n"
          . "\r\n" . $body;

    fwrite($s, $req);
    $resp = recv_all($s);
    fclose($s);

    $code = parse_status($resp);
    check('POST — valid response code', $code > 0,   (string)$code);
    check('POST — not 5xx',             $code < 500, (string)$code);
}

function test_content_length_zero(string $host, int $port): void {
    suite('Content-Length: 0 (no body hang)');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    check('CL:0 — server responds (no hang)',
          strlen($resp) > 0,
          'got no response — parser likely stuck in NEED_MORE');
    check('CL:0 — valid status code', parse_status($resp) > 0);
}

function test_leading_crlf(string $host, int $port): void {
    suite('Leading CRLF before request (telnet artifact)');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    $code = parse_status($resp);
    check('Leading CRLF — server responds',  strlen($resp) > 0);
    check('Leading CRLF — not 400',          $code !== 400, (string)$code);
}

function test_bad_request(string $host, int $port): void {
    suite('Malformed request → 400');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "GARBAGE NOT HTTP\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    check('Bad request — returns 400', parse_status($resp) === 400, substr($resp, 0, 40));
}

function test_missing_host(string $host, int $port): void {
    suite('Missing Host header (HTTP/1.1) → 400');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "GET / HTTP/1.1\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    check('Missing Host → 400', parse_status($resp) === 400, substr($resp, 0, 40));
}

function test_chunked_delivery(string $host, int $port): void {
    suite('Chunked delivery (8 bytes at a time)');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    $req  = "GET /chunked HTTP/1.1\r\nHost: localhost\r\n\r\n";
    $len  = strlen($req);
    for ($i = 0; $i < $len; $i += 8) {
        fwrite($s, substr($req, $i, 8));
        usleep(3000);   // 3 ms
    }

    $resp = recv_all($s);
    fclose($s);

    check('Chunked delivery — server responds',   strlen($resp) > 0);
    check('Chunked delivery — valid status code', parse_status($resp) > 0);
}

function test_keep_alive(string $host, int $port): void {
    suite('Keep-alive: 4 requests on one connection');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    for ($i = 1; $i <= 4; $i++) {
        $conn = ($i < 4) ? 'keep-alive' : 'close';
        fwrite($s,
            "GET /ka/php/{$i} HTTP/1.1\r\n"
            . "Host: localhost\r\n"
            . "Connection: {$conn}\r\n\r\n"
        );
    }

    $resp  = recv_all($s);
    fclose($s);
    $count = count_substr($resp, 'HTTP/1.');
    check('All 4 keep-alive responses received', $count === 4, "got {$count}");
}

function test_large_body(string $host, int $port): void {
    suite('Large body (64 KiB)');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    $body = str_repeat('P', 65536);
    $req  = "POST /large HTTP/1.1\r\n"
          . "Host: localhost\r\n"
          . "Content-Length: " . strlen($body) . "\r\n"
          . "\r\n" . $body;

    fwrite($s, $req);
    $resp = recv_all($s);
    fclose($s);

    $code = parse_status($resp);
    check('Large body — server responds', strlen($resp) > 0);
    check('Large body — not 5xx',         $code > 0 && $code < 500, (string)$code);
}

function test_pipelined(string $host, int $port): void {
    suite('Pipelined requests');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s,
        "GET /pipe/php/1 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        . "GET /pipe/php/2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    );
    $resp  = recv_all($s);
    fclose($s);
    $count = count_substr($resp, 'HTTP/1.');
    check('Both pipelined responses received', $count === 2, "got {$count}");
}

function test_http10(string $host, int $port): void {
    suite('HTTP/1.0');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    fwrite($s, "GET / HTTP/1.0\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    check('HTTP/1.0 — server responds',   strlen($resp) > 0);
    check('HTTP/1.0 — valid status code', parse_status($resp) > 0);
}

function test_content_length_variants(string $host, int $port): void {
    suite('Content-Length case variants');
    $variants = [
        'content-length', 'Content-Length', 'CONTENT-LENGTH', 'Content-length',
    ];
    foreach ($variants as $v) {
        $s = tcp_connect($host, $port);
        if (!$s) { check("connect ({$v})", false); continue; }

        $body = 'test';
        fwrite($s,
            "POST /case HTTP/1.1\r\nHost: localhost\r\n{$v}: " . strlen($body) . "\r\n\r\n{$body}"
        );
        $resp = recv_all($s);
        fclose($s);
        $code = parse_status($resp);
        check("CL variant '{$v}' — not 400", $code !== 400, (string)$code);
    }
}

function test_non_ascii_garbage(string $host, int $port): void {
    suite('Non-ASCII / binary garbage → 400');
    $s = tcp_connect($host, $port);
    if (!$s) { check('connect', false); return; }

    // Send raw binary bytes that are not valid HTTP
    fwrite($s, "\xff\xfe\x00\x01INVALID\r\n\r\n");
    $resp = recv_all($s);
    fclose($s);

    $code = parse_status($resp);
    check('Non-ASCII garbage — returns 400 or closes', $code === 400 || strlen($resp) === 0);
}

// ── main ──────────────────────────────────────────────────────────────────────

echo "PHP HTTP Parser Test Client → {$host}:{$port}\n";
echo str_repeat('=', 50) . "\n";

$probe = @fsockopen($host, $port, $errno, $errstr, 2.0);
if (!$probe) {
    fwrite(STDERR, "ERROR: Cannot connect to {$host}:{$port} — is the server running?\n");
    exit(1);
}
fclose($probe);

test_simple_get($host, $port);
test_post_with_body($host, $port);
test_content_length_zero($host, $port);
test_leading_crlf($host, $port);
test_bad_request($host, $port);
test_missing_host($host, $port);
test_chunked_delivery($host, $port);
test_keep_alive($host, $port);
test_large_body($host, $port);
test_pipelined($host, $port);
test_http10($host, $port);
test_content_length_variants($host, $port);
test_non_ascii_garbage($host, $port);

echo "\n" . str_repeat('=', 50) . "\n";
echo "  PHP results: {$pass}/{$total} passed";
if ($fail) echo "  ({$fail} failed)";
echo "\n" . str_repeat('=', 50) . "\n";

exit($fail > 0 ? 1 : 0);
