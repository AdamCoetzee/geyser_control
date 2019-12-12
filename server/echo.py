#SERVER

from socket import *

def server(address):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind(address)
    sock.listen(1)
    while True:
        client, addr = sock.accept()
        print('Connection from ', addr)
        handler(client)
        command = input()
        if "s_" in command:
            s_command(command)

def handler(client):
    while True:
        data = client.recv(128)
        if not data:
            break
        print(data)
        command = input()
        if "g_" in command:
            g_command(client, command)

        if "s_" in command:
            s_command(client, command)
            
    print('Connection closed')
    client.close()

def g_command(client, command):
    client.sendall(str.encode(command))

#def s_command(client, command):
#    if "exit" in command:
        

if __name__ == '__main__':
    while True:
        server(('', 4444))
