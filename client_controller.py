import getpass
import sys
import logging as log
import socket_client
import my_errors
import protocol_wrapper as protowrap


log.basicConfig(stream=sys.stdout, level=log.DEBUG,format='%(asctime)s - %(levelname)s - %(message)s')
""" supreme module controlling socket-thread and gui """


class ClientController:

    SERVER_ADDRESS = "127.0.0.1"
    SERVER_PORT = 54000

    def __init__(self):

        self.read_settings("settings.txt")
        try:
            self.internetClient = socket_client.Client(self.server_address, self.server_port)
            self.internetClient.get_client_controller(self)

        except my_errors.UnableToCreateSocket:
            self.handle_failed_socket_creation()

    def read_settings(self,file_name):
        try:
            with open(file_name,'r') as F:
                self.server_address = F.readline().strip()
                self.server_port = int(F.readline().strip())

        except FileNotFoundError as e:
            log.error(e)
            log.error("Loading default settings")
            self.load_default_settings()


    def load_default_settings(self):
        self.server_address = self.SERVER_ADDRESS
        self.server_port = self.SERVER_PORT


    def connect(self):
        try:
            self.internetClient.connect(1)
        except my_errors.UnableToConnect:
            log.error("Unable to Connect")#for now
            sys.exit()



    def start_send_receive_client(self):
        try:
            self.internetClient.start()
        except my_errors.ConnectionBrokenReceive:
            log.error("Server unexpectedly closed")
            sys.exit()

    def main_loop(self):
        while True:
            data = self.get_message()
            if data is not None:
                type,mess = data
                print(mess)
                if mess == "1":
                    print("request o hasło ilogin")
                    self.send_login_info("siadło,","podsiadło")



    def restart_send_receive_client(self):
        self.internetClient.start_running()

    def stop_send_receive_client(self):
        self.internetClient.stop_running()

    def get_login_info(self):
        print("ENTER LOGIN AND PASSWORD TO LOG IN")
        login = input("Login: ")
        # self.password = getpass.getpass()
        password = input('Password:')
        log.debug(f"LOGIN: {login} PASSWORD: {password}")


    def send_login_info(self,login,password):
        login_mes = protowrap.LoginMessage(login=login, password=password).get_message()
        login_len = len(login_mes)

        type = protowrap.LOGIN
        header = protowrap.Header(msgtype=type, msglength=login_len).get_message()

        full_message = header + login_mes  # strip('\n')
        log.debug(f"full login Message  {full_message}")
        self.internetClient.pass_message_to_send_buffer(full_message)

    def handle_failed_socket_creation(self):
        log.debug("Failed to create socket: program will close")
        self.internetClient.close_socket()
        sys.exit()

    def get_message(self):
        return self.internetClient.pop_message_from_receive_buffer()











