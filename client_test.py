import select
import socket
import selectors
import threading
import sys
import time
import logging as log
import my_errors
import utilities as util
import protocol_wrapper as protowrap



log.basicConfig(stream=sys.stdout, level=log.DEBUG,
                format='%(asctime)s - %(levelname)s - %(message)s')
#log.disable(log.CRITICAL)

HOST1 = "25.139.176.21"
HOST = "127.0.0.1"
PORT = 54000

LEN_OF_HEADER = 6 #on that many bytes length of header will come

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
        self.receive_buffer.get_send_buffer(self.send_buffer)


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
                self.receive()

            if self.client_socket in write_socket:
                self.send()



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


    def receive(self):

        try:
            message_len = self.receive_buffer.get_length_of_incoming_message()
            log.debug(f"{self.receive_buffer.bytes_to_receive}")
            r_data = self.client_socket.recv(message_len)

            log.debug(f"[{len(r_data)}] bytes received : {r_data} ")


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
        self.send_receive_loop()

    def stop_running(self):
        self.running.set_locked_bool(False)

    def get_client_controller(self,client_controller):
        self.client_controller = client_controller



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
            self.remove_from_buffer()


class Receive_buffer(util.Buffer_control):

    def __init__(self):
        util.Buffer_control.__init__(self)
        self.bytes_to_receive = LEN_OF_HEADER
        self.total_received = 0
        self.status = 0
        self.message = b"" # or b""
        self.type_of_incoming_message = None


    def read_from_buffer(self):
        return self.remove_from_buffer()


    def get_length_of_incoming_message(self):
        return self.bytes_to_receive - self.total_received

    def get_send_buffer(self,send_buffer):
        self.send_buffer = send_buffer

    def collect_message(self,message_part):
        self.message+= message_part
        self.total_received+=len(message_part)

        if self.total_received == self.bytes_to_receive:

            self.total_received = 0
            if self.status == 0:
                self.status = 1
                log.debug(f"received header: {self.message}")


                header = protowrap.Header(message=self.message.decode('utf-8'))# protowrap.Header(self.message.decode('utf-8))
                self.type_of_incoming_message = header.get_msg_type()
                self.bytes_to_receive = header.get_msg_length()
                log.debug(f"msgType {self.type_of_incoming_message} msgLength {self.bytes_to_receive}")


                """dummy code"""
                self.bytes_to_receive = 40
                self.received_whole_message()
                self.type_of_incoming_message = 0


            elif self.status == 1:
                self.status =0
                self.received_whole_message()
                self.bytes_to_receive = LEN_OF_HEADER

            self.total_received = 0
            self.message = b""  # b""


    def received_whole_message(self):

        if self.type_of_incoming_message == protowrap.STATEMENT:#1
            login_mes = protowrap.LoginMessage(login="chyba", password="kurwa dziala 69").get_message()

            log.debug(f"login mess: {login_mes} ")
            login_len = len(login_mes)
            type = protowrap.LOGIN
            header = protowrap.Header(msgtype=type,msglength=login_len).get_message()
            log.debug(f"header: {header}")
            full_message = header + login_mes #strip('\n')
            log.debug(f"full login message  {full_message}")
            self.send_buffer.write_to_buffer(full_message)




