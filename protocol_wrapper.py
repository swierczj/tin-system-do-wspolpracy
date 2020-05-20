
"""module responsible for translating protocol"""
import my_errors
HEADER_LENGTH = 6
""" Message types in header"""
LOGIN = 0
STATEMENT = 1
EDIT = 2
PUBLIC_KEY = 3


class Message:
    pass


class Header:
    def __init__(self, **kwargs):

        if 'message' in kwargs:
            self.message = kwargs.get('message')
            try:
                self.msgType = int(self.message[:2])
                self.msgLength = int(self.message[4:])
            except ValueError:
                raise my_errors.CorruptedHeader

        elif 'msglength' and 'msgtype' in kwargs:
            self.msgType = kwargs.get('msgtype')
            self.msgLength = kwargs.get('msglength')
            self.message = '0'*(2-len(str(self.msgType))) + str(self.msgType)
            self.message+= '0'*(4-len(str(self.msgLength))) +str(self.msgLength)

    def get_msg_type(self):
        return self.msgType

    def get_msg_length(self):
        return self.msgLength

    def get_message(self):
        return self.message


class LoginMessage(Message):

    def __init__(self,**kwargs):

        if 'Message' in kwargs:
            self.login,self.password = kwargs.get('Message').split()

        elif 'login' and 'password' in kwargs:
            self.login =  kwargs.get('login')
            self.password = kwargs.get('password')
            self.message = self.login +'\n' +self.password

    def get_length_of_message(self):
        return len(self.message)

    def get_message(self):
        return  self.message

    def get_login(self):
        return self.login

    def get_password(self):
        return self.message


class Statement(Message):
    pass









