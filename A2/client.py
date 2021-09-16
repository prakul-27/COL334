from os import read
from socket import *
from threading import *

port = 1111
ip = '127.0.0.1'

def register(username, send_sckt, rcv_sckt):
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
    print('Registration Successful!')
    return True

def send(rcpt, mssg, send_sckt):
    send_mssg = 'SEND ' + rcpt + '\nContent-length: ' + str(len(mssg)) + '\n\n' + mssg
    
    send_sckt.send(send_mssg.encode())
    rcvd_mssg = send_sckt.recv(1024).decode()

    if rcvd_mssg == 'SEND ' + rcpt + '\n\n':
        return True
    print(rcvd_mssg)

    return False

def read_cmd_line(send_sckt):
    while True:
        mssg = input()
        details = mssg.split()
        
        if len(details) <= 1 or details[0][0] != '@':
            print('Sorry, try again')
            continue
        
        recipient = details[0][1:]
        words = details[1:]
        mssg = ''
        for word in words:
            mssg += (word + ' ')

        if send(recipient, mssg, send_sckt) == True:
            print("Message delivered successfully")
        else:
            print('Some error, try again')

def read_FRWD_mssgs(rcv_sckt):
    while True:
        mssg = rcv_sckt.recv(1024).decode()
        elmts = mssg.split('\n')
        header = elmts[0].split()[0]
        sender = elmts[0].split()[1]

        if header != 'FORWARD':
            rcv_sckt.send('ERROR 103 Header Incomplete\n\n'.encode())
            continue
        
        print(sender+': '+elmts[3])
        rcv_sckt.send(('RECEIVED ' + sender + '\n\n').encode())

while True:
    username = input('username_name: ')
    if username == 'ALL':
        print('Reserved keyword, please try again')
        continue

    send_sckt = socket(AF_INET, SOCK_STREAM)
    rcv_sckt = socket(AF_INET, SOCK_STREAM)

    send_sckt.connect((ip, port))
    rcv_sckt.connect((ip, port))

    if register(username, send_sckt, rcv_sckt) == True:
        t1 = Thread(target = read_cmd_line,args = (send_sckt,))
        t2 = Thread(target = read_FRWD_mssgs,args = (rcv_sckt,))
        t1.start() 
        t2.start()
        t1.join()
        t2.join()
        break
    else:
        send_sckt.close()
        rcv_sckt.close()