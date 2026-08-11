
#!/bin/bash

for i in $(seq 1 10000); do
    echo "Request $i"
    curl -s -i \
        -H "Host: localhost" \
        http://localhost:8080/cgi-bin/cgi_test
    	# https://www.youtube.com/
	echo
done
