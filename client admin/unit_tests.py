import unittest
import protocol_wrapper as proto_wrap
import encryptor
import utilities as util
import socket_client

"""ENCRYPTION TESTS"""


class EncryptionTests(unittest.TestCase):

    def test_encrypt_decrypt(self):
        data = "encrypted decrypted \n data1"
        key = b'1234567890123456'

        encrypted = encryptor.encrypt(key,data)
        decrypted = encryptor.decrypt(key,encrypted)

        self.assertEqual(decrypted,data)

    def test_encryption_standard(self):
        data = "message 1"
        key = b'1234567890123456'
        encrypted_code = b'uWuoiYApeiS0i2cQ4tU7gQ=='

        encrypted = encryptor.encrypt(key, data)

        self.assertEqual(encrypted,encrypted_code)

"""HEADER TESTS"""


class ProtocolTests(unittest.TestCase):

    def test_header_code(self):
        header_should_be = "030010"

        header = proto_wrap.Header(msgtype=3,msglength=10).get_message()
        self.assertEqual(header,header_should_be)

    def test_header_decode(self):
        header_type_should_be = 3
        header_message_length_should_be = 10

        header = proto_wrap.Header(message="030010")
        self.assertEqual(header.get_msg_type(),header_type_should_be)
        self.assertEqual(header.get_msg_length(),header_message_length_should_be)

    def test_statement(self):
        statement_type = proto_wrap.Statement(message="1").get_statement_type()
        message_type = proto_wrap.Statement(statementType=1).get_message()


        self.assertEqual(statement_type,1)
        self.assertEqual(message_type,"1")

"RESOURCES USING LOCK TEST"


class LockedResourcesTests(unittest.TestCase):

    def test_removing_occurences(self):
        buffer = util.BufferControl()
        for i in range(3):
            buffer.add_to_buffer("data1")
        buffer.remove_all_occurences_of("data1")
        self.assertEqual((len(buffer.get_buffer())),0)

    def test_get_message_from_buffer(self):
        buffer = util.BufferControl()
        nothing =buffer.get_one_message_from_buffer()
        buffer.add_to_buffer("data")
        data = buffer.pop_one_message_from_buffer()
        empty = len(buffer.get_buffer())


        self.assertEqual(nothing,None)
        self.assertEqual(data,"data")
        self.assertEqual(empty,0)

"""SENDBUFFER TEST OFFLINE"""


class SendBufferTests(unittest.TestCase):

    def test_sending_message(self):
        sendBuffer = socket_client.SendBuffer()
        mes0 = sendBuffer.get_message()
        sendBuffer.write_to_buffer((1,"datał"))

        mes1 = sendBuffer.get_message()

        sendBuffer.update_on_total_sent(6)
        mes2 = sendBuffer.get_message()

        sendBuffer.update_on_total_sent(5)
        mes3 = sendBuffer.get_message()

        self.assertEqual(mes0,None)
        self.assertEqual(mes1,"010005datał")
        self.assertEqual(mes2,"datał")
        self.assertEqual(mes3, None)


if __name__ == '__main__':
    unittest.main()


