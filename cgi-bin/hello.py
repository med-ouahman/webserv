#!/usr/bin/env python3
import os
import time


print("""Status: 200 OK\r\nContent-Type: text/html\r\n\r\n""", end="")
print("""
HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 562\r\nHost: webserv\r\nContent-Type: text/html\r\n\r\n
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div class="name">
        <div id="h-dvi">div</div>buttom
    </div>
    <script>
        const div = document.getElementById("h-div");
        if (div === null) return;

        div.addEventListener("click", addElements);

        function addElements() {
            const div = this;
            div.innerHTML += "<p>Hello World</p>"
        }

    </script>
</body>
</html>
""")
