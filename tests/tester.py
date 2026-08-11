import socket
import time
import os

connections = []

def clear():
    os.system("cls" if os.name == "nt" else "clear")

def print_help(command=None):
    commands = {
        "connect": """
connect <count> [HOST] [PORT]

Open <count> new TCP connections with HOST (default 0.0.0.0) and PORT (default 8080).

Example:
    connect 1000
""",
        "send": """
send <start> <end> <bytes>

Send <bytes> bytes on every connection in the range.

Example:
    send 0 999 4096
""",
        "recv": """
recv <start> <end> [bytes]

Receive up to <bytes> bytes (default 4096) from each connection.

Examples:
    recv 0 99
    recv 0 99 1024
""",
        "close": """
close <start> <end>

Close all connections in the range.

Example:
    close 500 999
""",
        "list": """
list

Show the number of currently open connections.
""",
        "clear": """
clear

Clear the screen.
""",
        "quit": """
quit | exit

Close all remaining connections and exit.
"""
    }

    if command:
        command = command.lower()

        if command in commands:
            print(commands[command])
        elif command == "?" or command == "help":
            print_help()
        else:
            print(f"Unknown command: {command}")
            print("Type 'help' to see available commands.")
        return

    print("""
Available commands:

  connect <count> [HOST] [PORT]
      Open new TCP connections.

  send <start> <end> <bytes>
      Send bytes on connections in the range.

  recv <start> <end> [bytes]
      Receive bytes from connections in the range.

  close <start> <end>
      Close connections in the range.

  list
      Show the number of currently open connections.

  clear
      Clear the screen.

  help [command] | ?
      Display help, optionally for a single command.
      Example:
          help connect

  quit | exit
      Close all remaining connections and exit.
""")


def connect(count, HOST="0.0.0.0", PORT=8080):
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
                if len(cmd) == 3:
                    connect(int(cmd[1]), cmd[2], int(cmd[3]))
                else:
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
            case "help" | "?":
               print_help(cmd[1] if len(cmd) > 1 else None)

            case "clear":
                clear()

            case _:
                print("Unknown command")

    except Exception as e:
        print(e)

for s in connections:
    if s is not None:
        s.close()
