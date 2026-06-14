#!/usr/bin/env python3
import os
import time


print("Status: 200 OK\r\n", end="")
print("Content-Type: text/html\r\n\r\n", end="")

print("""
<!DOCTYPE html>
<html>
<head>
    <title>Dummy CGI</title>
</head>
<body>
    <h1>Hello from CGI!</h1>
    <p>This is a dummy Python CGI script.</p>
</body>
</html>
""", end="")