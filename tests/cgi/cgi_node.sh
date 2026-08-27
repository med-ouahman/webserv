
#!/bin/bash

for i in $(seq 1 1); do
    echo "Request $i"
    curl -s -i \
        -H "Host: localhost" \
        http://localhost:3000/cgi.js
    	# https://www.youtube.com/
	echo
done
