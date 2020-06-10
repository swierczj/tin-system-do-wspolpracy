import threading

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


class BufferControl:# buffor storing strings ready to be transmited bytes/strings not sure yet

    def __init__(self):
        self.buffer = []
        self.buffer_lock = threading.Lock()

    def add_to_buffer(self, data):
        with self.buffer_lock:
            self.buffer.append(data)

    def pop_one_message_from_buffer(self):
        with self.buffer_lock:
            if len(self.buffer)>0:
                return self.buffer.pop(0)
            else:
                return None

    def get_one_message_from_buffer(self):              # FIFO
        with self.buffer_lock:
            if len(self.buffer)>0:
                return self.buffer[0]
            else:
                return None

    def get_buffer(self):
        with self.buffer_lock:
            return self.buffer

    def remove_all_occurences_of(self,value):
        with self.buffer_lock:
            self.buffer = [data for data in self.buffer if data != value]

    def is_in_buffer(self,value):
        with self.buffer_lock:
            return value in self.buffer


class DictionaryControl:

    def __init__(self):
        self.dictionary = {}
        self.dictionary_lock = threading.Lock()

    def add_element(self,key,value):
        with self.dictionary_lock:
            self.dictionary[key] = value

    def get_dictionary(self):
        with self.dictionary_lock:
            return self.dictionary

    def is_in_dictionary(self,key):
        with self.dictionary_lock:
            return key in self.dictionary

    def pop_element(self,key):
        with self.dictionary_lock:
            if key in self.dictionary:
                return self.dictionary.pop(key)
            else:
                return None

    def get_element(self,key):
        with self.dictionary_lock:
            return self.dictionary[key]












