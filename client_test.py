import select
import socket
import selectors
import threading
import sys
import time
import logging as log
import my_errors
import utilities as util



log.basicConfig(stream=sys.stdout, level=log.DEBUG,
                format='%(asctime)s - %(levelname)s - %(message)s')
#log.disable(log.CRITICAL)

HOST1 = "25.139.176.21"
HOST = "127.0.0.1"
PORT = 54000

LEN_OF_HEADER = 4 #on that many bytes length of header will come

CONNECTION_ATTEMPTS = 3

MSG_LEN = 64


class Client(threading.Thread):

    def __init__(self,host,port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        self.create_socket()
        self.running = util.LockedBool(True)
        self.send_buffer = Send_buffer()
        self.receive_buffer = Receive_buffer()


    def run(self) -> None:

        self.send_receive_loop()


    def create_socket(self): # replace with 'with'
        try:
            self.client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except socket.error:
            log.debug("Unable to create socket. Terminating program" )
            raise my_errors.UnableToCreateSocket

    def close_socket(self):
        self.client_socket.close()


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
                raise my_errors.UnableToConnect
                #self.client_socket.close()
                #sys.exit()


    def send_receive_loop(self):
        sl = [self.client_socket] #  sl as in socket list but there is only one socket

        while self.is_running():

            read_socket,write_socket,exception_socket = select.select(sl,sl,sl)

            if self.client_socket in read_socket:
                self.receive(MSG_LEN)

            if self.client_socket in write_socket:
                self.send()
                log.debug("send")


            if self.client_socket in exception_socket:
                """what do we handle here?"""
                log.critical("Error on socket or smth")
                self.client_socket.close()
                sys.exit()



    def send(self):

        try:
            message = self.send_buffer.get_message()
            if message is not None:
                sent = self.client_socket.send(message.encode('utf-8'))
                self.send_buffer.update_on_total_sent(sent)

        except socket.error as e:
            log.error("Connection broken")
            log.error(e)
            raise my_errors.ConnectionBrokenSend


    def receive(self,mess_len):

        try:
            message_len = self.receive_buffer.get_length_of_incoming_message()
            r_data = self.client_socket.recv()

            log.debug(f"[{len(r_data)}] bytes received : {r_data.decode('utf-8')} ")

            if r_data == b"":
                """ handling closed connection from server """
                log.info("Server closed connection ")
                raise my_errors.ConnectionClosedByServer

            self.receive_buffer.collect_message(r_data)


        except socket.error as e:#ConnectionResetError
            log.error("Connection broken")
            log.error(e)
            raise my_errors.ConnectionBrokenReceive



    def is_running(self):
        return self.running.get_locked_bool()


    def start_running(self):
        self.running.set_locked_bool(True)


    def stop_running(self):
        self.running.set_locked_bool(False)



class Send_buffer(util.Buffer_control):

    def __init__(self):
        util.Buffer_control.__init__(self)
        self.total_sent = 0
        self.bytes_to_send = 0
        self.completed_sending = True
        self.current_message = None

    def write_to_buffer(self,message):
        self.add_to_buffer(message)


    def get_message(self):
        if self.completed_sending:
            self.current_message = self.get_one_message_from_buffer()
            if self.current_message is not None:
                self.bytes_to_send = len(self.current_message)
                self.completed_sending = False
                return self.current_message

        else:
            return self.current_message[self.total_sent:]


    def update_on_total_sent(self,sent):
        self.total_sent += sent
        if self.total_sent == self.bytes_to_send and self.total_sent > 0:
            self.total_sent = 0
            self.completed_sending = True


class Receive_buffer(util.Buffer_control):

    def __init__(self):
        util.Buffer_control.__init__(self)
        self.bytes_to_receive = LEN_OF_HEADER
        self.total_received = 0
        self.status = 0
        self.completed_receiving = True
        self.message = "" # or b""


    def read_from_buffer(self):
        return self.remove_from_buffer()


    def get_length_of_incoming_message(self):
        return self.bytes_to_receive - self.total_received

    def collect_message(self,message_part):
        self.message+= message_part
        self.total_received+=len(message_part)

        if self.total_received == self.bytes_to_receive:
            self.add_to_buffer(self.message)
            self.message = ""
            self.total_received = 0


    def update_bytes_to_receive(self):
        pass

    def received_whole_part(self):
        pass











log.debug('Client import succesful.')


"""
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
            raise my_errors.ConnectionBrokenSend

"""