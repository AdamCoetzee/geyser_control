#echo.py

import time
from socket import *

time_hour = 0
time_minute = 0

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

def time_check():
    localtime = time.localtime(time.time())
    if ((localtime[3] == time_hour) and (localtime[4] == time_minute)):
        print("lmaocow")    
    print("[status] Current Time: ", localtime[3], localtime[4],"TRIGGER: ", time_hour, time_minute)

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
            s_command(command)
            

    print('Connection closed')
    client.close()

def s_command(command):
    if "s_getTime" in command:
        localtime = time.localtime(time.time())
        print ("Local current time :", localtime[3], ":", localtime[4])

    if "s_setTime" in command:
        global time_hour
        global time_minute
        time_hour = input("hour: ")
        time_minute = input("minute: ")

def g_command(client, command):
    client.sendall(str.encode(command))

if __name__ == '__main__':
    while True:
        server(('', 4444))
