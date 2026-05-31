#!/usr/bin/env node
/**
 * test_parser.js — Node.js HTTP Parser Test Client
 * Uses net.Socket for raw TCP. No axios, no node-fetch, no http module.
 *
 * Usage: node test_parser.js [host] [port]
 */

'use strict';

const net  = require('net');

const HOST = process.argv[2] || '127.0.0.1';
const PORT = parseInt(process.argv[3] || '8080', 10);

let pass  = 0;
let fail  = 0;
let total = 0;

// ── helpers ───────────────────────────────────────────────────────────────────

function suite(name) {
  console.log(`\n── ${name} ──`);
}

function check(label, condition, detail = '') {
  ++total;
  if (condition) {
    ++pass;
    console.log(`  ✓  ${label}`);
  } else {
    ++fail;
    const d = detail ? ` (${String(detail).slice(0, 80)})` : '';
    console.log(`  ✗  FAIL: ${label}${d}`);
  }
}

/**
 * Open a raw TCP connection, send `request` (Buffer or string),
 * collect all response bytes until the socket closes or a 2-second
 * silence, then resolve with the raw response Buffer.
 */
function rawRequest(request, timeoutMs = 2500) {
  return new Promise((resolve) => {
    const chunks = [];
    const sock   = new net.Socket();
    let   timer  = null;

    const finish = () => {
      clearTimeout(timer);
      sock.destroy();
      resolve(Buffer.concat(chunks));
    };

    const resetTimer = () => {
      clearTimeout(timer);
      timer = setTimeout(finish, timeoutMs);
    };

    sock.setTimeout(timeoutMs);
    sock.connect(PORT, HOST, () => {
      sock.write(typeof request === 'string'
        ? Buffer.from(request, 'binary')
        : request);
      resetTimer();
    });

    sock.on('data',  (chunk) => { chunks.push(chunk); resetTimer(); });
    sock.on('end',   finish);
    sock.on('close', finish);
    sock.on('error', finish);
    sock.on('timeout', finish);
  });
}

/** Send bytes with a per-chunk delay (simulates slow network). */
function chunkedRequest(data, chunkSize, delayMs) {
  return new Promise((resolve) => {
    const chunks = [];
    const sock   = new net.Socket();
    let   timer  = null;
    let   pos    = 0;

    const finish = () => {
      clearTimeout(timer);
      sock.destroy();
      resolve(Buffer.concat(chunks));
    };

    const resetTimer = () => {
      clearTimeout(timer);
      timer = setTimeout(finish, 2500);
    };

    const sendNext = () => {
      if (pos >= data.length) return;
      const slice = data.slice(pos, pos + chunkSize);
      sock.write(slice);
      pos += chunkSize;
      if (pos < data.length) setTimeout(sendNext, delayMs);
    };

    sock.connect(PORT, HOST, () => {
      sendNext();
      resetTimer();
    });

    sock.on('data',  (c) => { chunks.push(c); resetTimer(); });
    sock.on('end',   finish);
    sock.on('close', finish);
    sock.on('error', finish);
  });
}

function parseStatus(raw) {
  const s  = raw.toString('latin1');
  const sp = s.indexOf(' ');
  if (sp < 0) return 0;
  return parseInt(s.slice(sp + 1, sp + 4), 10) || 0;
}

function countOccurrences(raw, needle) {
  const s = raw.toString('latin1');
  let count = 0;
  let pos   = 0;
  while ((pos = s.indexOf(needle, pos)) !== -1) { ++count; pos += needle.length; }
  return count;
}

// ── test cases ────────────────────────────────────────────────────────────────

async function testSimpleGet() {
  suite('Simple GET');
  const resp = await rawRequest('GET / HTTP/1.1\r\nHost: localhost\r\n\r\n');
  const code = parseStatus(resp);
  check('GET / returns 2xx or 3xx', code >= 200 && code < 400, code);
  check('Response non-empty',       resp.length > 0);
}

async function testPostWithBody() {
  suite('POST with body');
  const body = 'js_field=js_value&runtime=node';
  const req  = [
    `POST /echo HTTP/1.1`,
    `Host: localhost`,
    `Content-Type: application/x-www-form-urlencoded`,
    `Content-Length: ${body.length}`,
    ``,
    body,
  ].join('\r\n');
  const resp = await rawRequest(req);
  const code = parseStatus(resp);
  check('POST — valid response code', code > 0,   code);
  check('POST — not 5xx',             code < 500, code);
}

async function testContentLengthZero() {
  suite('Content-Length: 0 (no body hang)');
  const resp = await rawRequest(
    'POST /empty HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n'
 );
  check(
    'CL:0 — server responds (no hang)',
    resp.length > 0,
    'got no response — parser likely stuck in NEED_MORE'
 );
  check('CL:0 — valid status code', parseStatus(resp) > 0);
}

async function testLeadingCRLF() {
  suite('Leading CRLF before request (telnet artifact)');
  const resp = await rawRequest(
    '\r\nGET /telnet-crlf HTTP/1.1\r\nHost: localhost\r\n\r\n'
 );
  const code = parseStatus(resp);
  check('Leading CRLF — server responds', resp.length > 0);
  check('Leading CRLF — not 400',         code !== 400, code);
}

async function testBadRequest() {
  suite('Malformed request → 400');
  const resp = await rawRequest('NOT VALID HTTP AT ALL\r\n\r\n');
  check('Bad request — returns 400', parseStatus(resp) === 400, parseStatus(resp));
}

async function testMissingHost() {
  suite('Missing Host header (HTTP/1.1) → 400');
  const resp = await rawRequest('GET / HTTP/1.1\r\n\r\n');
  check('Missing Host → 400', parseStatus(resp) === 400, parseStatus(resp));
}

async function testChunkedDelivery() {
  suite('Chunked delivery (3 bytes at a time, 5 ms gap)');
  const req  = Buffer.from('GET /chunked HTTP/1.1\r\nHost: localhost\r\n\r\n');
  const resp = await chunkedRequest(req, 3, 5);
  check('Chunked delivery — server responds',   resp.length > 0);
  check('Chunked delivery — valid status code', parseStatus(resp) > 0);
}

async function testKeepAlive() {
  suite('Keep-alive: 5 requests on one connection');
  const chunks = [];
  const sock   = new net.Socket();

  await new Promise((resolve) => {
    let timer = null;
    const resetTimer = () => {
      clearTimeout(timer);
      timer = setTimeout(() => { sock.destroy(); resolve(); }, 2500);
    };

    sock.connect(PORT, HOST, () => {
      for (let i = 1; i <= 5; i++) {
        const conn = i < 5 ? 'keep-alive' : 'close';
        sock.write(`GET /ka/js/${i} HTTP/1.1\r\nHost: localhost\r\nConnection: ${conn}\r\n\r\n`);
      }
      resetTimer();
    });

    sock.on('data', (c) => { chunks.push(c); resetTimer(); });
    sock.on('end',   resolve);
    sock.on('close', resolve);
    sock.on('error', resolve);
  });

  const resp  = Buffer.concat(chunks);
  const count = countOccurrences(resp, 'HTTP/1.');
  check('All 5 keep-alive responses received', count === 5, `got ${count}`);
}

async function testLargeBody() {
  suite('Large body (64 KiB)');
  const body   = Buffer.alloc(65536, 0x4a); // 'J'
  const header = Buffer.from(
    `POST /large HTTP/1.1\r\nHost: localhost\r\nContent-Length: ${body.length}\r\n\r\n`
 );
  const resp = await rawRequest(Buffer.concat([header, body]), 5000);
  const code = parseStatus(resp);
  check('Large body — server responds', resp.length > 0);
  check('Large body — not 5xx',         code > 0 && code < 500, code);
}

async function testPipelined() {
  suite('Pipelined requests');
  const both =
    'GET /pipe/js/1 HTTP/1.1\r\nHost: localhost\r\n\r\n' +
    'GET /pipe/js/2 HTTP/1.1\r\nHost: localhost\r\n\r\n';
  const resp  = await rawRequest(both);
  const count = countOccurrences(resp, 'HTTP/1.');
  check('Both pipelined responses received', count === 2, `got ${count}`);
}

async function testHttp10() {
  suite('HTTP/1.0');
  const resp = await rawRequest('GET / HTTP/1.0\r\n\r\n');
  check('HTTP/1.0 — server responds',   resp.length > 0);
  check('HTTP/1.0 — valid status code', parseStatus(resp) > 0);
}

async function testHeaderCaseVariants() {
  suite('Content-Length case variants');
  const variants = [
    'content-length', 'Content-Length', 'CONTENT-LENGTH', 'cOnTeNt-LeNgTh',
  ];
  for (const v of variants) {
    const body = 'hi';
    const req  = `POST /case HTTP/1.1\r\nHost: localhost\r\n${v}: ${body.length}\r\n\r\n${body}`;
    const resp = await rawRequest(req);
    const code = parseStatus(resp);
    check(`CL variant '${v}' — not 400`, code !== 400, code);
  }
}

async function testNonAsciiGarbage() {
  suite('Non-ASCII / binary garbage → 400');
  const garbage = Buffer.from([0xff, 0xfe, 0x00, 0x01, 0x49, 0x4e, 0x56,
                                0x41, 0x4c, 0x49, 0x44, 0x0d, 0x0a, 0x0d, 0x0a]);
  const resp = await rawRequest(garbage);
  const code = parseStatus(resp);
  check('Binary garbage — returns 400 or closes', code === 400 || resp.length === 0, code);
}

async function testDeleteMethod() {
  suite('DELETE method');
  const resp = await rawRequest('DELETE /resource/99 HTTP/1.1\r\nHost: localhost\r\n\r\n');
  check('DELETE — valid response', parseStatus(resp) > 0);
}

// ── main ──────────────────────────────────────────────────────────────────────

async function main() {
  console.log(`Node.js HTTP Parser Test Client → ${HOST}:${PORT}`);
  console.log('='.repeat(50));

  // Connectivity probe
  try {
    await new Promise((resolve, reject) => {
      const s = new net.Socket();
      s.connect(PORT, HOST, () => { s.destroy(); resolve(); });
      s.on('error', reject);
      s.setTimeout(2000, () => reject(new Error('timeout')));
    });
  } catch {
    console.error(`ERROR: Cannot connect to ${HOST}:${PORT} — is the server running?`);
    process.exit(1);
  }

  await testSimpleGet();
  await testPostWithBody();
  await testContentLengthZero();
  await testLeadingCRLF();
  await testBadRequest();
  await testMissingHost();
  await testChunkedDelivery();
  await testKeepAlive();
  await testLargeBody();
  await testPipelined();
  await testHttp10();
  await testHeaderCaseVariants();
  await testNonAsciiGarbage();
  await testDeleteMethod();

  console.log(`\n${'='.repeat(50)}`);
  process.stdout.write(`  Node.js results: ${pass}/${total} passed`);
  if (fail) process.stdout.write(`  (${fail} failed)`);
  console.log(`\n${'='.repeat(50)}`);

  process.exit(fail > 0 ? 1 : 0);
}

main().catch((err) => {
  console.error('Unexpected error:', err);
  process.exit(1);
});
