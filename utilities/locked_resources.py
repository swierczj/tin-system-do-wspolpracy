import threading
import sys
import logging as log

log.basicConfig(stream=sys.stdout, level=log.DEBUG,
                format='%(asctime)s - %(levelname)s - %(message)s')

class LockedBool:

    def __init__(self,bool):
        self.bool = bool
        self.lock = threading.Lock()

    def get_locked_bool(self):
        with self.lock:
            return self.bool

    def set_locked_bool(self,new_bool_value):
        with self.lock:
            self.bool = new_bool_value



class Buffer_control:# buffor storing strings ready to be transmited bytes/strings not sure yet

    def __init__(self):
        self.message_buffer = []
        self.buffor_lock = threading.Lock()

    def add_to_buffer(self,message):
        with self.buffor_lock:
            self.message_buffer.append(message)

    def remove_from_buffer(self):
        with self.buffor_lock:
            if len(self.message_buffer)>0:
                return self.message_buffer.pop(0)
            else:
                return None

    def get_one_message_from_buffer(self):              # FIFO
        with self.buffor_lock:
            if len(self.message_buffer)>0:
                return self.message_buffer[0]
            else:
                return None











