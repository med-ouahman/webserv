
#!/bin/bash

for i in $(seq 1 1000); do
    echo "Request $i"
    curl -s -i \
        -H "Host: localhost" \
        http://localhost:8080/
    echo
done
