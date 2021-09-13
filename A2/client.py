from socket import *
from threading import *

port = int(input('Enter port number: '))
send_sckt = socket(AF_INET, SOCK_STREAM)
rcv_sckt = socket(AF_INET, SOCK_STREAM)

def register(username, server_name):
    send_mssg = 'REGISTER TOSEND ' + username + '\n\n'
    rcv_mssg = 'REGISTER TORECV ' + username + '\n\n'
    
    send_sckt.send(send_mssg.encode())
    send_mssg_rcvd = send_sckt.recv(1024).decode()
    print(send_mssg_rcvd)

    rcv_sckt.send(rcv_mssg.encode())
    rcv_mssg_rcvd = rcv_sckt.recv(1024).decode()
    print(rcv_mssg_rcvd)

    if send_mssg_rcvd != 'REGISTERED TOSEND ' + username + '\n\n':
        print(send_mssg_rcvd)
        return False 
    if rcv_mssg_rcvd != 'REGISTERED TORECV ' + username + '\n\n':
        print(rcv_mssg_rcvd)
        return False
    return True

first = False
while True:
    username = input('username_name: ')
    server_name = input('server_name: ')

    if first is False:
        send_sckt.connect((server_name, port))
        rcv_sckt.connect((server_name, port))
        first = True

    if username == 'ALL':
        print('Reserved keyword, please try again')
        continue

    if server_name == 'localhost':
        server_name = '127.0.0.1'

    if register(username, server_name) == True:
        break

def send(rcpt, mssg):
    send_mssg = 'SEND ' + rcpt + '\nContent-length: ' + str(len(rcpt)) + '\n' + mssg
    send_sckt.send(send_mssg.encode())
    rcvd_mssg = send_sckt.recv(1024).decode()

    if rcvd_mssg == 'SEND ' + rcpt + '\n' + '\n':
        return True
    print(rcvd_mssg)

    return False

def read_cmd_line():
    while True:
        mssg = input("Enter message: ")
        details = mssg.split()
        
        if len(details) <= 1 or details[0][0] != '@':
            print('Sorry, try again')
            continue
        
        recipient = details[0][1:]
        mssg = details[1]

        if send(recipient, mssg) == True:
            print("Message delivered successfully")
        else:
            print("Some error message")

def read_FRWD_mssgs():
    while True:
        mssg = rcv_sckt.recv(1024).decode()
        elmts = mssg.split('\n')
        header = elmts[0].split()[0]
        sender = elmts[0].split()[1]
        
        if header != 'FORWARD':
            rcv_sckt.send('ERROR 103 Header Incomplete\n\n'.encode())
            continue

        rcv_sckt.send(('RECEIVED ' + sender + '\n\n').encode())
    
t1 = Thread(target = read_cmd_line,args = ())
t2 = Thread(target = read_FRWD_mssgs,args = ())

t1.start()
t2.start()

t1.join()
t2.join()