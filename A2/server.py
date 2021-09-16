from socket import *
from os import *
from _thread import *

port = 1111
server_sckt = socket(AF_INET, SOCK_STREAM)
server_sckt.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
server_sckt.bind(('',port))
server_sckt.listen()
print('server is running')

client_list_send = {}
client_list_rcv = {}
is_socket_registered = []

def send(recpt, body, c):
    if recpt not in client_list_rcv and recpt != 'ALL':
        c.send('ERROR 102 Unable to send\n\n'.encode())
        return False
    sender = ''
    for sndr, snd_sckt in client_list_send.items():
        if snd_sckt == c:
            sender = sndr
            break    
    mssg = 'FORWARD '+sender+'\nContent-length: '+str(len(body))+'\n\n'+body
    print('recpt ' + recpt)
    if recpt == 'ALL':
        print('here')
        for _, rcv_sckt in client_list_rcv.items():
            rcv_sckt.send(mssg.encode())
            rcvd_mssg = rcv_sckt.recv(1024).decode()
            print('rcvd_mssg ', rcvd_mssg)
            if rcvd_mssg != 'RECEIVED '+sender+'\n\n':
                c.send('ERROR 102 Unable to send\n\n'.encode())
                return False
        return True
    else:
        rcv_sckt = client_list_rcv[recpt]
        rcv_sckt.send(mssg.encode())
        rcvd_mssg = rcv_sckt.recv(1024).decode()
        print(rcvd_mssg)
        if rcvd_mssg != 'RECEIVED '+sender+'\n\n':
            c.send('ERROR 102 Unable to send\n\n'.encode())
            return False
    c.send(('SEND '+str(recpt)+'\n\n').encode())
    return True
def wait(c):
    while True:
        mssg = c.recv(1024).decode().split('\n')
        recpt = mssg[0].split()[1]
        body = mssg[3]
        
        print(mssg)

        send(recpt, body, c)
def register_snd_sckt(mssg, c):
    if mssg[0].split()[0] == 'REGISTER' and mssg[0].split()[1] == 'TOSEND':
        if mssg[0].split()[2] in client_list_send.keys():
            c.send('USER ALREADY REGISTERED'.encode())
            return
        else:
            if mssg[0].split()[2].isalnum() == True:
                client_list_send[mssg[0].split()[2]] = c
                c.send(('REGISTERED TOSEND '+mssg[0].split()[2]+'\n\n').encode())
            else:
                c.send('ERROR 100 Malformed username\n\n'.encode())
                return
        wait(c)
def register_rcv_sckt(mssg, c):
    if mssg[0].split()[0] == 'REGISTER' and mssg[0].split()[1] == 'TORECV':
        if mssg[0].split()[2] in client_list_rcv.keys():
            c.send('USER ALREADY REGISTERED'.encode())
        else:
            if mssg[0].split()[2].isalnum() == True:
                client_list_rcv[mssg[0].split()[2]] = c
                c.send(('REGISTERED TORECV '+mssg[0].split()[2]+'\n\n').encode())
            else:
                c.send('ERROR 100 Malformed username\n\n'.encode())

while True:
    c, _ = server_sckt.accept()
    mssg = c.recv(1024).decode().split('\n')

    print(mssg)
    
    if mssg[0].split()[0] == 'REGISTER':
        is_socket_registered.append(c)
        start_new_thread(register_snd_sckt, (mssg, c,))
        start_new_thread(register_rcv_sckt, (mssg, c,))