import select
import socket
import selectors
import threading
import sys
import time

HOST = "127.0.0.1"
PORT = 55555


class Client(threading.Thread):

    def __init__(self,host,port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port


    def run(self) -> None:
        self.create_socket()
        self.connect()
        self.send_receive_loop()


    def create_socket(self): # replace with 'with'
        try:
            self.client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except socket.error:
            print("Unable to create socket.\n Terminating program ")
            sys.exit()


    def connect(self):
        try:
            self.client_socket.connect((self.host,self.port))
            self.client_socket.setblocking(0)
            print(f"Connection to  :succesful")
        except socket.error as e:
            print(f"Unable to connect:\n {e}\n")
            self.client_socket.close()
            sys.exit()


    def send_receive_loop(self):
        sl = [self.client_socket] #  sl as in socket list but there is only one socket
        i = 0
        k = 0
        while True:
            read_socket,write_socket,exception_socket = select.select(sl,sl,sl)

            for notified_socket in read_socket:
                print("ready to receive")
                time.sleep(0.2)
                data = notified_socket.recv(2)
                if data :
                    print(data.decode('utf-8'))
                if data == b"": # prolly cant to it like this
                    print("Closed connection")
                    sys.close()

            for notified_socket in write_socket:
                i+=1
                if i % 1000000 == 0:
                    print(k % 10000)
                    k+=1
                    message = f"{k}"
                    len = self.client_socket.send(message.encode('utf-8'))
                    print(f"Sent: {len}")



            for notified_socket in exception_socket:
                print("exception occured")
                sys.exit()
                #again idk









client = Client(HOST,PORT)
client.start()
