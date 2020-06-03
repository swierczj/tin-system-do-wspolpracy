import getpass
import time
import sys
import logging as log
import socket_client
import my_errors
import protocol_wrapper as protowrap
import threading
import utilities as util
from GUI import gui


"""SYSTEM MESSAGES"""

CREATE_ACCOUNT = "create account"
CREATE_ACCOUNT_DATA = "create account data"
DISCONNECTED = "disconnected"

GET_ACCOUNT_LIST = "get account list"
DELETE_ACCOUNT = "delete account"

"""SYSTEM VARIABLES"""

LOGIN = "login"
PASSWORD = "password"

log.basicConfig(stream=sys.stdout, level=log.DEBUG,format='%(asctime)s - %(levelname)s - %(message)s')
""" supreme module controlling socket-thread and gui """

class ClientController(threading.Thread):

    SERVER_ADDRESS = "127.0.0.1"
    SERVER_PORT = 54000

    def __init__(self):
        threading.Thread.__init__(self)
        self.read_settings("settings.txt")
        self.running = util.LockedBool(True)
        self.connected = util.LockedBool(False)
        self.systemMessages = util.BufferControl()
        self.systemVariables = util.DictionaryControl()
        self.create_internetClient()


    def read_settings(self,file_name):
        try:
            with open(file_name,'r') as F:
                self.server_address = F.readline().strip()
                self.server_port = int(F.readline().strip())

        except FileNotFoundError as e:
            log.error(e)
            log.error("Loading default settings")
            self.load_default_settings()


    def create_internetClient(self):
        try:
            self.internetClient = socket_client.Client(self.server_address, self.server_port)
            self.internetClient.get_client_controller(self)

        except my_errors.UnableToCreateSocket:
            self.handle_failed_socket_creation()


    def load_default_settings(self):
        self.server_address = self.SERVER_ADDRESS
        self.server_port = self.SERVER_PORT

    def add_gui_client(self,gui_client):
        self.gui_client = gui_client

    def stop_running(self):
        self.running.set_locked_bool(False)

    def is_running(self):
        return self.running.get_locked_bool()


    def connect(self):

        try:
            self.internetClient.connect(1)
            self.connected.set_locked_bool(True)

            #self.update_on_successful_login()

        except my_errors.UnableToConnect:
            log.error("Unable to Connect")
            self.connected.set_locked_bool(False)

    def reconnect(self):
        self.create_internetClient()
        self.connect()

        if self.connected.get_locked_bool():
            self.start_send_receive_client()
            self.update_on_connection()



    def start_send_receive_client(self):
        self.internetClient.start()

    def run(self) -> None:
        self.connect()

        if self.connected.get_locked_bool() ==True:
            self.update_on_connection()
            #self.update_on_login_request()


            self.start_send_receive_client()
        else:
            self.update_on_no_server_connection()
            #self.update_on_successful_login()  # TODO
        self.main_loop()


    def main_loop(self):
        while self.is_running():
            #print(self.connected.get_locked_bool(),self.systemMessages.get_buffer())

            if not self.connected.get_locked_bool() and self.systemMessages.is_in_buffer("reconnect"):
                self.handle_reconnect_request()

            if (not self.connected.get_locked_bool()) and self.systemMessages.is_in_buffer("disconnected"):
               self.handle_disconnected()

            if self.systemMessages.is_in_buffer("login") and (self.systemVariables.is_in_dictionary("login") and self.systemVariables.is_in_dictionary("password")):
                log.debug(self.systemVariables.get_dictionary())

                if not self.systemVariables.is_in_dictionary("waiting for login answer") and not self.systemVariables.is_in_dictionary("authenticated"):
                    login = self.systemVariables.get_element("login")
                    password = self.systemVariables.get_element("password")
                    self.send_login_info(login,password)
                    self.systemVariables.add_element("waiting for login answer",True)

                self.remove_system_message("login")

            if self.systemMessages.is_in_buffer(CREATE_ACCOUNT):
                self.send_create_account()


            if self.connected.get_locked_bool():
                self.read_incoming_message_and_process()




    def handle_reconnect_request(self):
        log.debug("Trying to reconnect")
        self.reconnect()
        self.remove_system_message("reconnect")

    def handle_disconnected(self):
        self.update_on_no_server_connection()
        self.remove_system_message(DISCONNECTED)


    def read_incoming_message_and_process(self):
        data = self.get_message()
        if data is not None:
            type, mess = data
            print(f"type: {type} payload: {mess}" )
            if type == protowrap.STATEMENT:
                statement = protowrap.Statement(message=mess).get_statement_type()

                if statement == protowrap.LOGIN_REQUEST:
                    log.debug("login request")
                    self.update_on_login_request()

                elif statement == protowrap.LOGIN_REJECTED:
                    self.update_on_failed_login()

            elif type == protowrap.CLIENT_ID:
                given_id = protowrap.ClientId(mess)
                self.systemVariables.add_element("id",given_id)
                self.update_on_successful_login()

            elif type == protowrap.ACCOUNT_LIST:
                print("got list")



    def restart_send_receive_client(self):
        self.internetClient.start_running()

    def stop_send_receive_client(self):
        self.internetClient.stop_running()

    def send_login_info(self,login,password):
        login_mes = protowrap.LoginMessage(login=login, password=password).get_message()

        type = protowrap.LOGIN
        full_message =  (type,login_mes)
        log.debug(f"full login Message  {full_message} len = {len(full_message)}")
        self.internetClient.pass_message_to_send_buffer(full_message)

    def handle_failed_socket_creation(self):
        log.debug("Failed to create socket: program will close")
        self.internetClient.close_socket()
        sys.exit()

    def get_message(self):
        return self.internetClient.pop_message_from_receive_buffer()

    def send_message(self,message):
        self.internetClient.pass_message_to_send_buffer(message)

    def update_on_connection(self):
        self.gui_client.update_on_connection()

    def update_on_login_request(self):
        self.gui_client.update_on_login_request()


    def update_on_successful_login(self):
        log.debug("login successful")
        self.write_system_variable("authenticated", True)
        self.gui_client.update_on_successful_login()
        self.systemVariables.pop_element("waiting for login answer")


    def send_create_account(self):
        print(self.systemVariables.get_dictionary())
        username,password = self.systemVariables.pop_element(CREATE_ACCOUNT_DATA)
        create_account_message = protowrap.CreateAccount(username,password).get_message()
        full_message = (protowrap.CREATE_ACCOUNT,create_account_message)
        print("full account:" , full_message ,len(full_message))
        self.send_message(full_message)



        self.remove_system_message(CREATE_ACCOUNT)


    def update_on_failed_login(self):
        log.debug("login failed")
        self.gui_client.update_on_failed_login()
        self.systemVariables.pop_element("waiting for login answer")

    def close_client_and_end_work(self):#odpowiednio pozamykać
        self.stop_running()
        self.stop_send_receive_client()
        log.debug("Closing clients")

    def log_out(self):
        self.systemVariables.pop_element("authenticated")
        self.systemVariables.pop_element(LOGIN)
        self.systemVariables.pop_element(PASSWORD)
        logout_message = protowrap.Statement(statementType=protowrap.LOG_OUT).get_message()
        self.send_message(protowrap.STATEMENT,logout_message)

        print("no co ty tato")
        #send logout

    def write_system_message(self,system_message):
        self.systemMessages.add_to_buffer(system_message)

    def remove_system_message(self, system_message):
        self.systemMessages.remove_all_occurences_of(system_message)


    def signal_connected(self):
        self.connected.set_locked_bool(True)

    def signal_on_lost_connection(self):
        self.connected.set_locked_bool(False)
        self.write_system_message("disconnected")


    def write_system_variable(self,key,value):
        self.systemVariables.add_element(key,value)

    def update_on_no_server_connection(self):
        self.gui_client.update_on_no_server_connection()












