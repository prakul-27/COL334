from socket import *
from threading import *

port = 1111
send_sckt = socket(AF_INET, SOCK_STREAM)
rcv_sckt = socket(AF_INET, SOCK_STREAM)

def register(username, server_name):
    send_mssg = 'REGISTER TOSEND ' + username
    rcv_mssg = 'REGISTER TORECV ' + username
    
    send_sckt.connect((server_name, port))
    send_sckt.send(send_mssg.encode())
    send_mssg_rcvd = send_sckt.recv(1024).decode()
    
    rcv_sckt.connect((server_name, port))
    rcv_sckt.send(rcv_mssg.encode())
    rcv_mssg_rcvd = rcv_sckt.recv(1024).decode()

    if send_mssg_rcvd != 'REGISTERED TOSEND ' + username:
        print(send_mssg_rcvd)
        return False 
    if rcv_mssg_rcvd != 'REGISTERED TORECV ' + username:
        print(rcv_mssg_rcvd)
        return False

    return True

while True:
    username = input('username_name: ')
    server_name = input('server_name: ')

    if server_name == 'localhost':
        server_name = '127.0.0.1'

    if register(username, server_name) == True:
        break

def send(rcpt, mssg):
    send_mssg = 'SEND ' + rcpt + '\nContent-length: ' + str(len(rcpt)) + '\n' + mssg
    send_sckt.send(send_mssg.encode())
    rcvd_mssg = send_sckt.recv(1024).decode() # maybe, rcv_sckt here?

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

        if send() == True:
            print("Message delivered successfully")
        else:
            print("Some error message")

def read_FRWD_mssgs():
    rcv_sckt.listen(1)

    return

t1 = Thread(target = read_cmd_line,args = ())
t2 = Thread(target = read_FRWD_mssgs,args = ())

t1.start()
t2.start()

t1.join()
t2.join()