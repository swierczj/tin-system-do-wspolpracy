
LOGIN = 0
STATEMENT = 1
EDIT = 2
PUBLIC_KEY = 3

#serializacja szyfyzacjia dostarczalność
class message:
    pass

class Header(message):
    def __init__(self, **kwargs):

        if 'message' in kwargs:
            self.message = kwargs.get('message')
            try:
                self.msgType = int(self.message[:2])
                self.msgLength = int(self.message[4:])
            except ValueError:
                pass

        elif 'msglength' and 'msgtype' in kwargs:
            self.msgType = kwargs.get('msgtype')
            self.msgLength = kwargs.get('msglength')
            self.message = '0'*(2-len(str(self.msgType))) + str(self.msgType)
            self.message+= '0'*(4-len(str(self.msgLength))) +str(self.msgLength)

    def get_msg_type(self):
        return self.msgType


    def get_msg_length(self):
        return  self.msgLength


    def get_message(self):
        return self.message


class LoginMessage(message):

    def __init__(self,**kwargs):

        if 'message' in kwargs:
            self.login,self.password = kwargs.get('message').split()

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


class Statement(message):
    pass


loginmes = LoginMessage(login='byq1',password='byq2')
print(loginmes.get_length_of_message())
login2= LoginMessage(message=loginmes.get_message())
print(login2.get_login())








