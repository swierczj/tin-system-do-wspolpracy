
#"AES/ECB/PKCS5Padding"
from Crypto.Cipher import AES
from Crypto.Util import Padding
from Crypto.Random import get_random_bytes
import base64

key = 'Sixteen byte key'.encode('utf-8')
BLOCKSIZE = AES.block_size





def create_random_key(key_length):
    return get_random_bytes(key_length)


def encrypt(key,data):
    if type(data) != bytes:
        data = data.encode('utf-8')
    cipher = AES.new(key, AES.MODE_ECB)

    data_padded = Padding.pad(data,BLOCKSIZE)
    data_encrypted = cipher.encrypt(data_padded)
    data_encrypted_coded = base64.b64encode(data_encrypted)
    return data_encrypted_coded


def decrypt(key,encrypted_data):
    cipher = AES.new(key, AES.MODE_ECB)
    data_encrypted_decoded = base64.b64decode(encrypted_data)
    data_decrypted = cipher.decrypt(data_encrypted_decoded)
    data = Padding.unpad(data_decrypted,BLOCKSIZE)
    return data



class Encryptor:
    pass

