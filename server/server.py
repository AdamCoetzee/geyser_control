#SERVER
import threading
import time
import calendar
import datetime
from socket import *

EXIT_STATUS = 0;    #flag for program termination
s_conn = [];        #list of connected clients
s_addr = [];        #corresponding list of addresses

def main():
    global EXIT_STATUS
    serverCmd_thread = threading.Thread(target=serverCommand)
    serverCmd_thread.start()     

    print('starting server...')
    
    HOST = ''
    PORT = 4444
    #'listening socket' object 
    sock = socket(AF_INET, SOCK_STREAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    sock.bind((HOST, PORT))

    sock.listen(1)
    
    while True:
        print('initial bounded socket listening for connection...')
        client, addr = sock.accept()    #with client a socket object
        print('socket connection initiated from ', addr)
        client.settimeout(25)
        while True:
            try:
                data = client.recv(128)
                print('arduino: ', data)
                if b'gs_' in data:
                    gs_command(client, data)
            except timeout:
                print('socket timed out')          
                break
     
        print('closing soccket  connection...')
        client.shutdown(SHUT_RDWR)
        client.close()

    sock.shutdown(SHUT_RDWR)
    sock.close()
    print('initial socket closed.')

    serverCmd_thread.join();
    exit()

#handling ONLY commands issued through server console
def serverCommand():
    while True:
        command = input()
        if "g_" in command:
            g_command(client, command)

        elif "s_" in command:
            s_command(command)

        else:
            print('invalid command')

#commands sent from the arduino to the server
def gs_command(client, command): 
    if b'fetchTime' in command:
        g_command(client, "g_sendTime")
    elif b'conConfirm' in command:  #received confirmation from arduino
        print('confirm :D')
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
def s_command(command):
    if "getTime" in command:
        now = datetime.datetime.now()
        print(f"Current time is : {now.hour}:{now.minute}")    

    elif "exit" in command:
        print('terminating program...')
        EXIT_STATUS = 1;

    else: 
        print('invalid s command')    
      
if __name__ == '__main__':
    main()
