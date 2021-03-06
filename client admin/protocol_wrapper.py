import my_errors
"""module responsible for translating protocol"""

HEADER_LENGTH = 6
"""SPECIAL"""
KEEP_ALIVE =66


""" Message types in header"""
LOGIN = 0
STATEMENT = 1
EDIT = 2
PUBLIC_KEY = 3
CLIENT_ID = 4
FILES = 5

CREATE_ACCOUNT = 6
EDIT_ACCOUNT = 7
DELETE_ACCOUNT = 8
ACCOUNT_LIST = 9

"STATEMENTS TYPES"
KEEP_ALIVE_2 = 0
LOGIN_REQUEST = 1
LOGIN_ACCEPTED = 2
LOGIN_REJECTED = 3
LOG_OUT = 4
END_OF_WORK = 5
PUBLIC_KEY_REQUEST = 6
FILE_LIST_REQUEST = 7

ACCOUNT_LIST_REQUEST = 8
ACCOUNT_CHANGE_ACCEPTED = 9
ACCOUNT_CHANGE_DECLINED = 10


def keep_alive():
    return "000000"




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


class LoginMessage:

    def __init__(self,**kwargs):

        if 'Message' in kwargs:
            self.message = kwargs.get('Message')
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


class BasicMessage:
    def __init__(self):
        self.message = None

    def convert_to_string_message(self,attribute_list):
        if len(attribute_list) <=1:
            self.message = str(attribute_list[0])
        else:
            self.message = str(attribute_list[0])
            for i in range(1,len(attribute_list)):
                self.message += "\n" + str(attribute_list[i])

    def get_message(self):
        return self.message

    def get_message_header_type(self):
        pass


class Statement:

    def __init__(self,**kwargs):

        if "message" in kwargs:
            try:
                self.type = int(kwargs.get("message"))
            except ValueError:
                raise ValueError

        elif "statementType" in kwargs:
            self.message = str(kwargs.get("statementType"))

    def get_statement_type(self):
        return self.type

    def get_message(self):
        return self.message


class ClientId:
    def __init__(self,ID):
        try:
            self.id = int(ID)
        except ValueError:
            raise ValueError

    def get_id(self):
        return self.id


class CreateAccount(BasicMessage):
    def __init__(self,login,password):
        BasicMessage.__init__(self)
        attributes = [login,password]
        self.convert_to_string_message(attributes)


class EditAccount(BasicMessage):
    def __init__(self,old_login,new_login,new_password):
        BasicMessage.__init__(self)
        attributes = [old_login,new_login, new_password]
        self.convert_to_string_message(attributes)


class DeleteAccount(BasicMessage):
    def __init__(self,login):
        BasicMessage.__init__(self)
        attributes = [login]
        self.convert_to_string_message(attributes)


class AccountList:
    def __init__(self,account_list_message):
        self.account_list = account_list_message.split()

    def get_account_list(self):
        return self.account_list






