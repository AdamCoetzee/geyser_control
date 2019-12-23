#SERVER
import threading
import time
import calendar
import datetime
from socket import *

def server(address):
    print('starting server...')
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind(address)
    sock.listen(1)
    print('listening for connection...')
    while True:
        client, addr = sock.accept()
        print('Connection from ', addr)
        run = handler(client)
    sock.close()
    exit()

def handler(client):
    serverCmd_thread = threading.Thread(target=serverCommand, args=[client])
    serverCmd_thread.start()                
    while True:
        time.sleep(0.2)
        data = client.recv(128)
        if not data:
            break
        print('arduino: ', data)
        if b'gs_' in data:
            gs_command(client, data)
       
    print('Connection closed')
    client.close()
    return False    

#handling ONLY commands issued through server console
def serverCommand(client):
    while True:
        command = input()
        if "g_" in command:
            g_command(client, command)

        elif "s_" in command:
            s_command(client, command)

        else:
            print('invalid command')

#commands sent from the arduino to the server
def gs_command(client, command): 
    if b'fetchTime' in command:
        g_command(client, "g_sendTime");
    else:
        print('invalid request from arduino')    

#commands sent directly to the arduino from server console
def g_command(client, command): 
    if "sendTime" in command:
        epoch = int(calendar.timegm(time.gmtime()))
        epoch = "g_setTime "+str(epoch)
        print('updating arduino time: ', epoch)
        client.sendall(str.encode(epoch))
    else:
        client.sendall(str.encode(command))

#commands issued to the server from server console
def s_command(client, command):
    if "getTime" in command:
        now = datetime.datetime.now()
        print(f"Current time is : {now.hour}:{now.minute}")    

    else: 
        print('invalid s command')    
      
if __name__ == '__main__':
    while True:
        server(('', 4444))
