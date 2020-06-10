import select
import socket
import selectors
import threading
import sys
import time
import enum
import logging as log
import my_errors
import utilities as util
import protocol_wrapper as protowrap

log.basicConfig(stream=sys.stdout, level=log.DEBUG,format='%(asctime)s - %(levelname)s - %(message)s')
#log.basicConfig(stream=sys.stdout, level=log.DEBUG,
 #               format='%(asctime)s - %(levelname)s - %(Message)s')
#log.disable(log.CRITICAL)

HOST1 = "25.139.176.21"
HOST = "127.0.0.1"
PORT = 54000

CONN_WAITING_TIME = 1

HEADER_LENGTH = protowrap.HEADER_LENGTH

CONNECTION_ATTEMPTS = 3

KEEP_ALIVE_TIME = 40
KEEP_ALIVE_HALF_TIME = 20




class Client(threading.Thread):

    def __init__(self,host,port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        self.client_controller = None

        self.create_socket()

        self.running = util.LockedBool(True)
        self.work_end_flag = util.LockedBool(False)
        self.send_buffer = SendBuffer()
        self.receive_buffer = ReceiveBuffer()

        self.keep_alive_last_time = 0


    def run(self) -> None:
        try:
            self.send_receive_loop()
        except (my_errors.ConnectionBrokenReceive,my_errors.ConnectionBrokenSend,my_errors.ConnectionClosedByServer):
            log.debug("Ending Socket Client")
            self.stop_running()
            self.close_socket()
            self.client_controller.signal_on_lost_connection()
        except my_errors.TimeoutError:
            log.debug("SERVER TIMEOUT - no response from server")
            self.stop_running()
            self.close_socket()
            self.client_controller.signal_on_lost_connection()


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
                self.client_socket.setblocking(1)
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



    def send_receive_loop(self):
        sl = [self.client_socket]

        self.keep_aliver = KeepAlive(self)
        while self.is_running():

            self.keep_aliver.update()
            read_socket,write_socket,exception_socket = select.select(sl,sl,sl)

            if self.client_socket in read_socket:
                try:
                    self.receive()
                    self.keep_aliver.update_on_received_anything() #TODO
                except my_errors.ConnectionBrokenReceive:
                    raise my_errors.ConnectionBrokenReceive

            if self.client_socket in write_socket:
                try:
                    self.send()
                except my_errors.ConnectionBrokenSend:
                    raise my_errors.ConnectionBrokenSend

            if self.client_socket in exception_socket:
                """what do we handle here?"""
                log.critical("Error on socket or smth")
                self.client_socket.close()
                raise my_errors.SocketError

            if self.work_end_flag.get_locked_bool():
                if self.send_buffer.get_message() is None:
                    self.stop_running()

    def send(self):
        try:
            message = self.send_buffer.get_message()

            if message is not None:

                sent = self.client_socket.send(message.encode('utf-8'))
                self.send_buffer.update_on_total_sent(sent)

        except socket.error as e:
            log.error("Connection broken send")
            log.error(e)
            raise my_errors.ConnectionBrokenSend

    def receive(self):

        try:
            message_len = self.receive_buffer.get_length_of_incoming_message()
            log.debug(f"Expecting  {self.receive_buffer.bytes_to_receive} bytes")
            r_data = self.client_socket.recv(message_len)
            log.debug(f"[{len(r_data)}] bytes received : {r_data} ")

            if r_data == b"":
                """ handling closed connection from server """
                log.info("Server closed connection ")
                raise my_errors.ConnectionClosedByServer

            self.receive_buffer.collect_message(r_data.decode('utf-8'))

            self.keep_alive_last_time = time.time()

        except socket.error as e:#ConnectionResetError
            log.error("Connection broken receive")
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

    def get_message_from_receive_buffer(self):
        return self.receive_buffer.get_one_message_from_buffer()

    def pop_message_from_receive_buffer(self):
        return self.receive_buffer.pop_one_message_from_buffer()

    def pass_message_to_send_buffer(self,message):
        self.send_buffer.write_to_buffer(message)

    def end_work(self):
        if self.is_alive():
            end_of_work_message = protowrap.Statement(statementType=protowrap.END_OF_WORK).get_message()
            full_message = (protowrap.STATEMENT,end_of_work_message)
            self.send_buffer.write_to_buffer(full_message)
            log.debug(f"sending end of work")
        self.work_end_flag.set_locked_bool(True)



class SendBuffer(util.BufferControl):

    def __init__(self):
        util.BufferControl.__init__(self)
        self.total_sent = 0
        self.bytes_to_send = 0
        self.completed_sending = True
        self.current_message = None

    def write_to_buffer(self,message):
        self.add_to_buffer(message)

    def get_message(self):
        if self.completed_sending:
            raw_message = self.get_one_message_from_buffer()
            if raw_message is not None:#type + message
                print(f"Sending message raw{raw_message}")

                type,data = raw_message
                """HANDLING KEEPALIVE"""
                if type == protowrap.KEEP_ALIVE:
                    log.debug("SENDING KEEP ALIVE")
                    self.bytes_to_send = HEADER_LENGTH
                    self.current_message = protowrap.keep_alive()
                    return self.current_message
                """//HANDLING KEEPALIVE"""

                data_len = len(data)
                header = protowrap.Header(msgtype=type,msglength=data_len).get_message()

                self.current_message = header + data
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
            self.pop_one_message_from_buffer()


class ReceiveBuffer(util.BufferControl):

    def __init__(self):
        util.BufferControl.__init__(self)
        self.bytes_to_receive = HEADER_LENGTH
        self.total_received = 0
        self.status = 0
        self.message = ""
        self.type_of_incoming_message = None
        self.status = State.waiting_for_header

    def read_from_buffer(self):
        return self.pop_one_message_from_buffer()

    def get_length_of_incoming_message(self):
        return self.bytes_to_receive - self.total_received

    def collect_message(self,message_part):
        self.message+= message_part
        self.total_received+=len(message_part)

        if self.total_received == self.bytes_to_receive:

            if self.status == State.waiting_for_header:
                self.status = State.waiting_for_message

                try:
                    header = protowrap.Header(message=self.message)#.decode('utf-8')
                    self.type_of_incoming_message = header.get_msg_type()
                    self.bytes_to_receive = header.get_msg_length()
                except my_errors.CorruptedHeader:
                    self.bytes_to_receive = 0
                    log.error("Corrupted header")
                    #notify about erroe



                if self.bytes_to_receive == 0:
                    self.status = State.waiting_for_header
                    self.bytes_to_receive = HEADER_LENGTH # it caouses erreo 0!!!!!!!!!!!! tu poprawić jak excepty bd robić

                log.debug(f" Header: msgType: {self.type_of_incoming_message} msgLength: {self.bytes_to_receive}")

            elif self.status == State.waiting_for_message:
                self.status =State.waiting_for_header
                self.received_whole_message()
                self.bytes_to_receive = HEADER_LENGTH

            self.total_received = 0
            self.message = ""  # b""

    def write_to_buffer(self, message):
        self.add_to_buffer(message)

    def received_whole_message(self):

        message_type_mess = (self.type_of_incoming_message,self.message)
        self.write_to_buffer(message_type_mess)
        print(f"State of receive buffer {self.get_buffer()}")
        #notify the controller somehow


class State(enum.Enum):
    waiting_for_header = 0
    waiting_for_message = 1

log.debug("Client imported succesfully")


class KeepAlive:
    def __init__(self,socket_client):
        self.socket_client = socket_client
        self.keep_alive_last_time = time.time()
        self.half_timeout_time = KEEP_ALIVE_HALF_TIME
        self.timeout_time = KEEP_ALIVE_TIME
        self.sent_keepalive = False

    def update(self):
        if self.sent_keepalive == False:
            if time.time() - self.keep_alive_last_time > self.half_timeout_time:
                keep_alive_message = (protowrap.KEEP_ALIVE,protowrap.keep_alive())
                self.socket_client.send_buffer.add_to_buffer(keep_alive_message)
                self.sent_keepalive = True
        else:
            if (time.time() - self.keep_alive_last_time > self.timeout_time) and self.sent_keepalive == True:
                raise my_errors.TimeoutError

    def update_on_received_anything(self):
        self.keep_alive_last_time = time.time()
        self.sent_keepalive = False

