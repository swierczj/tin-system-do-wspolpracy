import client_controller


c_ct = client_controller.ClientController()

c_ct.connect()

c_ct.start_send_receive_client()
c_ct.main_loop()












