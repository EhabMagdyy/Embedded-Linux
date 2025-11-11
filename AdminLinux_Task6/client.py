import socket

from common import HOST, PORT, MSG_SIZE

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
    client_socket.connect((HOST, PORT))

    while True:
        msg = input("Enter operation (e.g. 5 + 3), or 'exit' to quit: ")
        if msg.lower() == 'exit':
            break

        client_socket.sendall(msg.encode())
        result = client_socket.recv(MSG_SIZE).decode()
        print("Result:", result)
