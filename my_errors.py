

class UnableToConnect(Exception):
    pass


class UnableToCreateSocket(Exception):
    pass


class ConnectionBrokenReceive(Exception):
    pass


class ConnectionBrokenSend(Exception):
    pass


class ConnectionClosedByServer(Exception):
    pass


class InvalidLoginData(Exception):
    pass


class CorruptedHeader(Exception):
    pass