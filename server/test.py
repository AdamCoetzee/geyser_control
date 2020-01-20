#SERVER RE-WRITE
import threading
from socket import *
from enum import Enum
import time

#SERVER INFO
SERVER_STATE = 0
HOST = ''
PORT = 4444

#lists for storing client info and handling clients
sock_clients = []
addr_clients = []
thr_clients  = []
type_clients = []

def main():
    global SERVER_STATE
    print("starting server...")
    servCmdThr = threading.Thread(target=serverCommand)
    servCmdThr.start()
    
    servCmdThr.join()   

def serverCommand():
    global SERVER_STATE
    while SERVER_STATE == 0:
        cmd = input()
        if "exit" in cmd:
            print("terminating server...")
            SERVER_STATE = 1
        else:
            print("unknown server command.")

def acceptClients():
    global SERVER_STATE
    listener = socket(AF_INET, SOCK_STREAM) #listening socket
    listener.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    listener.bind((HOST, PORT))
    listener.listen(1)
    while SERVER_STATE == 1:
        client, addr = listener.accept()       
         
 
if __name__ == '__main__':
    main()
