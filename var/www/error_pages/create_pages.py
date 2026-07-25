from pathlib import Path

TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" href="../assets/icons/vilgax.ico">
    <title>VilgaX</title>
</head>
<body>
    <h1>{code} {reason}</h1>
    <div class="img">
        <img src="/images/vilgax.png" />
    </div>
    <h2>VilgaX version 1.0</h2>
    <style>
        body {{
            background: #000;
            color: aquamarine;
        }}

        #line {{
            margin: 10px 0;
            width: 100%;
            height: 2px;
            background-color: #000;
            text-align: center;
        }}

        h1, h2 {{
            text-align: center;
            margin-top: 2rem;
        }}

        .img {{
            width: 100%;
            height: 350px;
            display: flex;
            align-items: center;
            justify-content: center;
        }}

        .img img {{
            width: 350px;
            height: 350px;
        }}

    </style>
</body>
</html>
"""

ERRORS = {
    400: "Bad Request",
    401: "Unauthorized",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    408: "Request Timeout",
    409: "Conflict",
    411: "Length Required",
    413: "Payload Too Large",
    414: "URI Too Long",
    415: "Unsupported Media Type",
    429: "Too Many Requests",
    500: "Internal Server Error",
    501: "Not Implemented",
    502: "Bad Gateway",
    503: "Service Unavailable",
    504: "Gateway Timeout",
    505: "HTTP Version Not Supported",
}

OUTPUT_DIR = Path(".")

for code, reason in ERRORS.items():
    html = TEMPLATE.format(code=code, reason=reason)
    (OUTPUT_DIR / f"{code}.html").write_text(html, encoding="utf-8")
    print(f"Generated {code}.html")

print("Done.")
