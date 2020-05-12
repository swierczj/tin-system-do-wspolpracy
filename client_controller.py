import getpass
import sys
import logging as log
import client_test
import my_errors


log.basicConfig(stream=sys.stdout, level=log.DEBUG,
                format='%(asctime)s - %(levelname)s - %(message)s')


""" supreme module controlling socket-thread and gui """

class Client_controller:

    SERVER_ADDRESS = "127.0.0.1"
    SERVER_PORT = 54000

    def __init__(self):
        self.read_settings("settings.txt")

        try:
            self.internet_client = client_test.Client(self.server_address,self.server_port ) # !!!!! watch out for incorrect formats of both addres and port
        except my_errors.UnableToCreateSocket:
            log.debug("Options: end work or work offline for now no offline work close program")
            self.internet_client.close_socket()
            sys.exit()


    def read_settings(self,file_name):
        try:
            with open(file_name,'r') as F:
                self.server_address = F.readline().strip()
                self.server_port = int(F.readline().strip())


        except FileNotFoundError as e:
            log.error(e)
            log.error("loading default settings")
            self.load_default_settings()


    def load_default_settings(self):
        self.server_address = self.SERVER_ADDRESS
        self.server_port = self.SERVER_PORT


    def connect(self):
        try:
            self.internet_client.connect(1)
        except my_errors.UnableToConnect:
            log.error("Unable to Connect")
            sys.exit()



    def get_login_info(self):
        self.login = input("Login: ")
        # self.password = getpass.getpass()
        self.password = input('Password:')
        log.debug(f"{self.login} {self.password}")


    def start_send_receive_client(self):
        self.internet_client.start()

    def stop_send_receive_client(self):
        self.internet_client.stop_running()








