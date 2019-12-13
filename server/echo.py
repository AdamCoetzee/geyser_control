#SERVER
import threading
import time
from socket import *

def server(address):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind(address)
    sock.listen(1)
    while True:
        client, addr = sock.accept()
        print('Connection from ', addr)
        run = handler(client)
    sock.close()
    exit()

def handler(client):
    cmd_thread = threading.Thread(target=runcommand, args=[client])
    cmd_thread.start()                
    while True:
        time.sleep(0.2)
        data = client.recv(128)
        if not data:
            break
        print(data)
       
    print('Connection closed')
    client.close()
    return False    

def runcommand(client):
    while True:
        command = input()
        if "g_" in command:
            g_command(client, command)

        if "s_" in command:
            s_command(client, command)
        


def g_command(client, command):
    client.sendall(str.encode(command))

#def s_command(client, command):
          
if __name__ == '__main__':
    while True:
        server(('', 4444))
