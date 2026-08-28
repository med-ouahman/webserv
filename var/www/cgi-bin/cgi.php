<?php
fwrite(fopen("php://stderr", "w"), "This is stderr\n");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
<?php

echo "Hello from PHP";

echo "script filename: " getenv("SCRIPT_FILENAME");
echo "redirect status: " getenv("REDIRECT_STATUS");
?>
</body>
</html>
