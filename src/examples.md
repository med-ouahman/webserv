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
COnnection: keep-alive 

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

## CGI TESTS:

```http
POST /cgi-bin/hello.py HTTP/1.1
Host: ss
```
```


## POST
- POST doesn't require a body, it just accepts one if present

## routing erros

### when indexing, allowed methods probably aren't checked or checked in the wrong location conf

```
```
POST /post_body HTTP/1.1
Host: ss
Content-Length: 41
```
```
