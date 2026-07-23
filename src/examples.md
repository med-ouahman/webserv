# Telnet HTTP Examples

## Static GET (works just fine)

```http
GET / HTTP/1.1
Host: ss
Connection: close

```

## Keep-Alive (finally workssssssss!!!)

```http
GET / HTTP/1.1
Host: ss
Connection: keep-alive

GET / HTTP/1.1
Host: ss

GET / HTTP/1.0
Host: ss
Connection: keep-alive

GET / HTTP/1.0
Host: ss
Connection: close
```

## 404 (works just fine)

```http
GET /does-not-exist HTTP/1.1
Host: ss
Connection: close

```

## Bad Method (works just fine)

```http
PATCH / HTTP/1.1
Host: ss
Connection: close

```

## Upload POST (internal nigger error)

```http
POST /upload.txt HTTP/1.1
Host: ss
Connection: close
Content-Length: 11

hello world
```

## POST Missing Length (works just fine)

```http
POST /upload.txt HTTP/1.1
Host: ss
Connection: close

```

## Missing Host (works just fine "bad request")

```http
GET / HTTP/1.1
Connection: close

```

## HTTP/1.0 Close (works just fine)

```http
GET / HTTP/1.0
Host: ss

```

## HTTP/1.0 Keep-Alive (i have no clue, it kept alive but didn't respond with anything)

```http
GET / HTTP/1.0
Host: ss
Connection: keep-alive

GET / HTTP/1.0
Host: ss
Connection: close

```
