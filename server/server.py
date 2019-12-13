#ARDUINO CONTROL SERVER
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((socket.gethostname(), 4444))
s.listen(1)

while True:
    client, address = s.accept()
    print(f"Connection from {address} has been established.")
    
