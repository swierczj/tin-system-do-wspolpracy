import select
import socket
import selectors
import threading
import sys
import time
import logging as log


log.basicConfig(stream=sys.stdout, level=log.DEBUG,
                format='%(asctime)s - %(levelname)s - %(message)s')
#log.disable(log.CRITICAL)

HOST1 = "25.139.176.21"
HOST = "127.0.0.1"
PORT = 54000

CONNECTION_ATTEMPTS = 2

MSG_LEN = 64


class Client(threading.Thread):

    def __init__(self,host,port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port


    def run(self) -> None:
        self.create_socket()
        self.connect(CONNECTION_ATTEMPTS)
        self.send_receive_loop()


    def create_socket(self): # replace with 'with'
        try:
            self.client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except socket.error:
            log.debug("Unable to create socket. Terminating program" )
            sys.exit()


    def connect(self,no_attempts):
        for it in range(no_attempts):
            try:
                self.client_socket.connect((self.host,self.port))
                self.client_socket.setblocking(0)

                log.debug(f"Connection to {self.host}:{self.port} succesful")
                break

            except socket.error as e:
                log.debug(f"Attempt:[{it}] Connection to {self.host}:{self.port} failed.")
                log.debug(f"{e}")

                if(it < no_attempts-1):
                    continue
                self.client_socket.close()
                sys.exit()


    def send_receive_loop(self):
        sl = [self.client_socket] #  sl as in socket list but there is only one socket

        i = 0 #dummy message generator
        k = 0

        while True:
            i+=1 #dummy iterator
            read_socket,write_socket,exception_socket = select.select(sl,sl,sl)

            if self.client_socket in read_socket:
                self.receive(MSG_LEN)

            if self.client_socket in write_socket:
                if i % 1000000 == 0:
                    k+=1
                    message = f"ADMIN:[{k}]"
                    self.send(message)
                if k >= 8 :
                    log.debug("Client ends its work")
                    self.client_socket.close()
                    sys.exit()

            if self.client_socket in exception_socket:
                """what do we hande here?"""
                log.critical("Error on socket or smth")
                self.client_socket.close()
                sys.exit()



    def send(self,message):
        try:
            total_sent = 0
            while total_sent < len(message):
                sent = self.client_socket.send(message[total_sent:].encode('utf-8'))
                if sent == 0:
                    log.error("Lost connection")

                log.debug(f"[{sent}] bytes sent: -> {message[total_sent:]} ")
                total_sent += sent

        except socket.error as e:
            log.error("Connection broken")
            log.error(e)
            self.client_socket.close()
            sys.exit()

    def receive(self,mess_len):

        try:
            r_data = self.client_socket.recv(MSG_LEN)
            # do something with data: append/check etc.
            log.debug(f"[{len(r_data)}] bytes received : {r_data.decode('utf-8')} ")

            if r_data == b"":
                """ handling closed connection from server """
                log.info("Server closed connection ")
                self.client_socket.close()
                time.sleep(10)
                sys.exit()

        except socket.error as e:#ConnectionResetError
            log.error("Connection broken")
            log.error(e)
            self.client_socket.close()
            sys.exit()
            #self.client_socket.close()


    """ czy napewno potrzbne"""
    def recoonnect(self):
        pass


log.debug('Client import succesful.')
