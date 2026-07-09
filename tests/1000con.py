import socket
import time
import os

connections = []

h = input("ip (x.x.x.x): ")
p = input("port: ")

HOST = h if len(h) != 0 else "127.0.0.1"
PORT = int(p) if len(p) != 0 else 3000


def clear():
    os.system("cls" if os.name == "nt" else "clear")

def print_help():
    print("""
Available commands:

  connect <count>
      Open <count> new TCP connections.
      Example:
          connect 1000

  send <start> <end> <bytes>
      Send <bytes> bytes on every connection in the range.
      Example:
          send 0 999 4096

  recv <start> <end> [bytes]
      Receive up to <bytes> bytes (default 4096) from each connection.
      Examples:
          recv 0 99
          recv 0 99 1024

  close <start> <end>
      Close all connections in the range.
      Example:
          close 500 999

  list
      Show the number of currently open connections.

  help | ?
      Display this help message.

  quit | exit
      Close all remaining connections and exit.
""")

def connect(count):
    for _ in range(count):
        try:
            s = socket.create_connection((HOST, PORT))
            s.setblocking(False)
            connections.append(s)
        except Exception as e:
            print(e)
            break

    print(f"{len(connections)} total connections.")


def send_range(start, end, size):
    data = b'x' * size

    for i in range(start, min(end + 1, len(connections))):
        s = connections[i]
        if s is None:
            continue
        try:
            s.sendall(data)
        except Exception as e:
            print(f"[{i}] {e}")


def recv_range(start, end, size=4096):
    for i in range(start, min(end + 1, len(connections))):
        s = connections[i]
        if s is None:
            continue
        try:
            data = s.recv(size)
            print(f"[{i}] received {len(data)} bytes")
        except Exception as e:
            print(f"[{i}] {e}")


def close_range(start, end):
    for i in range(start, min(end + 1, len(connections))):
        s = connections[i]
        if s is None:
            continue
        try:
            s.close()
        finally:
            connections[i] = None


def list_connections():
    alive = sum(1 for s in connections if s is not None)
    print(f"{alive}/{len(connections)} open")

print_help()
while True:
    try:
        cmd = input("> ").split()
    except EOFError:
        break

    if not cmd:
        continue

    try:
        match cmd[0]:
            case "connect":
                connect(int(cmd[1]))

            case "send":
                send_range(
                    int(cmd[1]),
                    int(cmd[2]),
                    int(cmd[3])
                )

            case "recv":
                if len(cmd) == 4:
                    recv_range(
                        int(cmd[1]),
                        int(cmd[2]),
                        int(cmd[3])
                    )
                else:
                    recv_range(
                        int(cmd[1]),
                        int(cmd[2])
                    )

            case "close":
                close_range(
                    int(cmd[1]),
                    int(cmd[2])
                )

            case "list":
                list_connections()

            case "quit" | "exit":
                break
            case "help":
                print_help()

            case "clear":
                clear()

            case _:
                print("Unknown command")

    except Exception as e:
        print(e)

for s in connections:
    if s is not None:
        s.close()