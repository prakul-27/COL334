from socket import *
from threading import *

port = int(input('Enter port number: '))
server_sckt = socket(AF_INET, SOCK_STREAM)
server_sckt.bind(('',port))
server_sckt.listen()
print('server is running')

client_list_send = {}
client_list_rcv = {}
is_socket_registered = []

def send(recpt, body, c):
    if client_list_rcv[recpt] not in is_socket_registered:
        c.send('ERROR 102 Unable to send\n\n'.encode())
        return False
    sender = ''
    for sndr, snd_sckt in client_list_send:
        if snd_sckt == c:
            sender = sndr
            break
    if recpt == 'ALL':
        for username, rcv_sckt in client_list_rcv:
            mssg = 'FORWARD '+sender+'\nContent-length: '+len(body)+'\n\n'+body
            rcv_sckt.send(mssg.encode())
            rcvd_mssg = rcv_sckt.recv(1024).decode()
            if rcvd_mssg != 'RECEIVED '+sender+'\n\n':
                c.send('ERROR 102 Unable to send\n\n'.encode())
                return False
        return True
    else:
        rcv_sckt = client_list_rcv[recpt]
        mssg = 'FORWARD '+sender+'\nContent-length: '+len(body)+'\n\n'+body
        rcv_sckt.send(mssg.encode())
        rcvd_mssg = rcv_sckt.recv(1024).decode()
        if rcvd_mssg != 'RECEIVED '+sender+'\n\n':
            c.send('ERROR 102 Unable to send\n\n'.encode())
            return False
    return True

def wait():    
    while True:
        c, addr = server_sckt.accept()
        mssg = c.recv(1024).decode().split('\n')

        if mssg[0].split()[0] == 'SEND':
            recpt = mssg[0].split()[1]
            body = mssg[3]
            if send(recpt, body, c) == True:
                break
    return

def register_snd_sckt(mssg, c):
    if mssg[0].split()[0] == 'REGISTER' and mssg[0].split()[1] == 'TOSEND':
        if mssg[0].split()[2] in client_list_send.keys():
            c.send('USER ALREADY REGISTERED'.encode())
        else:
            if mssg[0].split()[2].isalnum() == True:
                client_list_send[mssg[0].split()[2]] = c
                c.send(('REGISTERED TOSEND '+mssg[0].split()[2]+'\n\n').encode())
            else:
                c.send('ERROR 100 Malformed username\n\n'.encode())
        #c.close()        
        #wait()

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
        #c.close()
while True:
    c, addr = server_sckt.accept()
    mssg = c.recv(1024).decode().split('\n')    

    print(mssg)

    if mssg[0].split()[0] in ['SEND','RECEIVED','FORWARD'] and c not in is_socket_registered:
        c.send('ERROR 101 No user registered\n\n'.encode())
        continue        

    if mssg[0].split()[0] == 'REGISTER':
        is_socket_registered.append(c)
        t1 = Thread(target=register_snd_sckt, args=(mssg,c,)) # for snd_sckt
        t2 = Thread(target=register_rcv_sckt, args=(mssg,c,)) # for rcv_sckt
        t1.start()
        t2.start()
        t2.join()
        t1.join()

    print(client_list_send)
    print(client_list_rcv)