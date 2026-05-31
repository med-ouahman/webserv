import socket
import sys

def main(argv) -> int:
    if len(argv) < 2:
        print(f"Usage:\npython3 {argv[0]} ip port")
        return 1
    s = socket.socket()
    s.connect((argv[1], int(argv[2])))
    requests = (
        "GET /ka/cpp/1 HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        "GET /ka/cpp/2 HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        "GET /ka/cpp/3 HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        "GET /ka/cpp/4 HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
   )
    
    s.sendall(requests.encode())
    
    response = b""
    while True:
        chunk = s.recv(4096)
        print(chunk)
        if not chunk:
            break
        response += chunk
    
    print(response.decode())
    s.close()
    return 0

main(sys.argv)