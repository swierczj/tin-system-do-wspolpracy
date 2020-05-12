import passwd_pb2
import protocol_pb2
from Crypto import Cipher

#serializacja szyfyzacjia dostarczalność
class message:
    pass


class Login_message(message):


    def __init__(self, message):
        self.login_data = passwd_pb2.login_data()
        self.message = message
        self.parse_to_object()

    def __init__(self,login,password):
        self.login_data = passwd_pb2.login_data()
        self.login_data.login = login
        self.password = password
        self.message = self.serialize()

    def __init__(self,login: str = None, password: str =None):
        self.login_data = passwd_pb2.login_data()
        self.login_data.login = login
        self.password = password
        self.message = self.serialize()



    def serialize(self):
        return self.login_data.SerializeToString()


    def parse_to_object(self):
        self.login_data.ParseFromString(self.message)

    def get_length_of_message(self):
        return len(self.message)




