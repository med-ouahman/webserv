
for i in {0..1000}
do
curl -i -X POST http://localhost:8080/directory/youpi/.bla \
     -H "Content-Type: text/plain" \
     --data "w" 
done
