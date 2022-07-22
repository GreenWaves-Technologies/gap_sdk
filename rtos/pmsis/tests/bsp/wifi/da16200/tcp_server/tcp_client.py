# TCP client
import socket
from time import sleep

HOST = '192.168.115.102' #'DA16200_A762'   # The remote host
PORT = 5228              # The same port as used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    sleep(1)
    while True:
        #print('Send to server')
        s.sendall(b'OK') #Hello from client')
        data = s.recv(2048)
        #print('Received from server: ', repr(data))
