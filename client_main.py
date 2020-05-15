import client_controller
import time


c_ct = client_controller.Client_controller()

c_ct.internet_client.get_client_controller(c_ct)


c_ct.connect()
c_ct.start_send_receive_client()


time.sleep(10)
c_ct.stop_send_receive_client()


#c_ct.get_login_info()









