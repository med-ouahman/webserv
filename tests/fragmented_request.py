#!/usr/bin/env python3
"""Probe an HTTP server with one request split across multiple TCP writes.

This tests TCP fragmentation, not HTTP ``Transfer-Encoding: chunked``.  TCP does
not preserve write boundaries, but a delay between writes makes separate reads
by the server much more likely and is useful when debugging incremental parsers.
"""

from __future__ import annotations

import argparse
import random
import socket
import sys
import time
from dataclasses import dataclass


REQUEST = (
    b"GET / HTTP/1.1\r\n"
    b"Host: localhost\r\n"
    b"User-Agent: fragmented-request-probe/1.0\r\n"
    b"Accept: */*\r\n"
    b"\r\n"
)


@dataclass
class Result:
    name: str
    outcome: str
    elapsed: float
    response: bytes
    error: str = ""


def escaped(data: bytes, limit: int = 70) -> str:
    text = repr(data)[2:-1]
    if len(text) > limit:
        return text[: limit - 3] + "..."
    return text


def fixed_chunks(data: bytes, size: int) -> list[bytes]:
    return [data[i : i + size] for i in range(0, len(data), size)]


def random_chunks(data: bytes, seed: int, maximum: int = 8) -> list[bytes]:
    rng = random.Random(seed)
    chunks: list[bytes] = []
    offset = 0
    while offset < len(data):
        size = rng.randint(1, min(maximum, len(data) - offset))
        chunks.append(data[offset : offset + size])
        offset += size
    return chunks


def semantic_chunks() -> list[bytes]:
    return [
        b"GET ",
        b"/ ",
        b"HTTP/1.1\r",
        b"\nHost: local",
        b"host\r",
        b"\nUser-Agent: fragmented-request-probe/1.0\r\n",
        b"Accept: */*\r",
        b"\n\r",
        b"\n",
    ]


def response_complete(data: bytes) -> bool:
    marker = data.find(b"\r\n\r\n")
    if marker < 0:
        return False

    header = data[:marker].lower()
    body = data[marker + 4 :]
    for line in header.split(b"\r\n")[1:]:
        if line.startswith(b"content-length:"):
            try:
                length = int(line.split(b":", 1)[1].strip())
            except ValueError:
                return True
            return len(body) >= length

    # Receiving a complete header is enough for this parser-fragmentation probe.
    return True


def run_case(
    host: str,
    port: int,
    name: str,
    chunks: list[bytes],
    delay: float,
    timeout: float,
    verbose: bool,
) -> Result:
    response = bytearray()
    started = time.monotonic()

    try:
        with socket.create_connection((host, port), timeout=timeout) as sock:
            sock.settimeout(timeout)
            for index, chunk in enumerate(chunks, 1):
                if verbose:
                    print(
                        f"    send {index:02}/{len(chunks):02}: "
                        f"{len(chunk):3} bytes  {escaped(chunk)}"
                    )
                sock.sendall(chunk)
                if index != len(chunks) and delay:
                    time.sleep(delay)

            while not response_complete(response):
                block = sock.recv(4096)
                if not block:
                    break
                response.extend(block)

    except socket.timeout:
        return Result(name, "TIMEOUT", time.monotonic() - started, bytes(response))
    except (ConnectionError, OSError) as exc:
        return Result(
            name,
            "SOCKET_ERROR",
            time.monotonic() - started,
            bytes(response),
            str(exc),
        )

    elapsed = time.monotonic() - started
    if not response:
        return Result(name, "CLOSED_NO_RESPONSE", elapsed, b"")
    if not response_complete(response):
        return Result(name, "INCOMPLETE_RESPONSE", elapsed, bytes(response))
    return Result(name, "RESPONSE", elapsed, bytes(response))


def status_line(response: bytes) -> str:
    if not response:
        return ""
    return response.split(b"\r\n", 1)[0].decode("latin-1", errors="replace")


def build_cases(request: bytes, seed: int) -> list[tuple[str, list[bytes]]]:
    request_line_end = request.find(b"\r\n") + 2
    return [
        ("single write (control)", [request]),
        ("two writes: split after first byte", [request[:1], request[1:]]),
        ("two writes: split after request line",
         [request[:request_line_end], request[request_line_end:]]),
        ("two writes: final CRLF separate", [request[:-2], request[-2:]]),
        ("one write per HTTP line", request.splitlines(keepends=True)),
        ("semantic boundaries, including split CRLF", semantic_chunks()),
        ("fixed 13-byte writes", fixed_chunks(request, 13)),
        ("fixed 5-byte writes", fixed_chunks(request, 5)),
        ("fixed 2-byte writes", fixed_chunks(request, 2)),
        ("one-byte writes", fixed_chunks(request, 1)),
        (f"random 1..8-byte writes (seed {seed})", random_chunks(request, seed)),
    ]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Send a valid GET request using controlled TCP write fragments."
    )
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8080)
    parser.add_argument("--delay", type=float, default=0.01,
                        help="seconds between writes (default: 0.01)")
    parser.add_argument("--timeout", type=float, default=2.0,
                        help="connect/response timeout in seconds (default: 2.0)")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--verbose", action="store_true",
                        help="print every fragment before sending it")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    cases = build_cases(REQUEST, args.seed)

    print(f"Target: {args.host}:{args.port}")
    print(f"Request size: {len(REQUEST)} bytes")
    print(f"Delay between writes: {args.delay:.6f}s")
    print()

    results: list[Result] = []
    for name, chunks in cases:
        print(f"[{name}] {len(chunks)} write(s)")
        result = run_case(
            args.host,
            args.port,
            name,
            chunks,
            args.delay,
            args.timeout,
            args.verbose,
        )
        results.append(result)
        detail = status_line(result.response) or result.error or "no response bytes"
        print(f"  {result.outcome:18} {result.elapsed:7.3f}s  {detail}")

    print("\nSummary")
    for result in results:
        marker = "PASS" if result.outcome == "RESPONSE" else "FAIL"
        print(f"  {marker:4}  {result.name}: {result.outcome}")

    failures = [result for result in results if result.outcome != "RESPONSE"]
    if failures:
        print(
            "\nThe control request and fragmented requests contain identical bytes; "
            "only the socket write pattern differs."
        )
        return 1
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        print("\nInterrupted", file=sys.stderr)
        raise SystemExit(130)
