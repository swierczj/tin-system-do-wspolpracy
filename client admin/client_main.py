import client_controller
import GUI


c_ct = client_controller.ClientController()
gui_client = GUI.GuiAdminApp()
c_ct.add_gui_client(gui_client)
gui_client.add_client_controller(c_ct)

c_ct.start()
gui_client.mainloop()
c_ct.close_client_and_end_work()



















