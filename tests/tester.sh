
#!/bin/bash

for i in $(seq 1 10); do
      	echo "\n\n\n\n\n\n\n\n\n" | ./tester http://localhost:8080
	echo "Test $i: $?" >> test_results.txt
done
