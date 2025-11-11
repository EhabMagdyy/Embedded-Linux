import socket

from common import HOST, PORT, MSG_SIZE

def calculate(num1, num2, op):
    if op == '+':
        return num1 + num2
    elif op == '-':
        return num1 - num2
    elif op == '*':
        return num1 * num2
    elif op == '/':
        return num1 / num2 if num2 != 0 else 'Error: divide by zero'
    elif op == '%':
        return num1 % num2 if num2 != 0 else 'Error: divide by zero'
    else:
        return 'Invalid operation'

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    server_socket.bind((HOST, PORT))    # Bind to address and port
    server_socket.listen()              # Listen for incoming connections
    print(f"Server listening on {HOST}:{PORT}...")

    conn, addr = server_socket.accept() # Accept a connection
    with conn:
        print(f"Connected by {addr}")
        # Handle client requests
        while True:
            data = conn.recv(64).decode()
            if not data:
                break 

            try:
                num1, op, num2 = data.split()
                num1, num2 = float(num1), float(num2)
                result = calculate(num1, num2, op)
            except Exception as e:
                result = f"Error: {e}"

            conn.sendall(str(result).encode())  # Send back the result
