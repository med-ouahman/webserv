#!/usr/bin/env python3

import os
import time

print("""Content-Type: text/html\r\n\rf""", end="")
print("""
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
