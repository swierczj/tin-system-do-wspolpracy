import tkinter as tk
import time

APP_NAME = 'Admin App'
SCREEN_SIZE = "600x500+500+150" # width_height right down
POPUP_SCREEN_SIZE = "400x200+600+175"

#TODO OBSŁUŻENIE POWRACANIA DO LOGOWANIA
# ZROBIENIE DODAWANIA KLIENTÓW
# TODO KONTO STWÓRZ NOWE /ZMIEŃ HASŁO




#amind class >> containter frame >> pages
class GuiAdminApp(tk.Tk):

    def __init__(self):
        tk.Tk.__init__(self)
        self.setup()

        container = tk.Frame(self)
        container.grid(row=0,column=0)#pack(side="top",fill="both",expand=True)
        #container.grid_rowconfigure(0,weight=1)
        #container.grid_columnconfigure(0,weight=1)

        self.menubar = MenuBar(self)



        self.frames = {} #dict

        for F in (StartPage, Administration, LoginPage):
            frame = F(container,self)
            self.frames[F] = frame
            frame.grid(row=0,column=0,sticky="nsew")

        self.show_frame(StartPage)

    def add_client_controller(self,client_controller):
        self.client_controller = client_controller


    def show_frame(self,page):
        frame = self.frames[page]
        frame.tkraise()


    def setup(self):
        self.title(APP_NAME)
        self.geometry(SCREEN_SIZE)


    def update_on_connection(self):
        self.show_frame(StartPage)
        self.frames[StartPage].update_on_connected_to_server()


    def update_on_login_request(self):
        self.show_frame(LoginPage)

    def update_on_successful_login(self):
        self.show_frame(Administration)

    def update_on_failed_login(self):
        self.show_frame(LoginPage)
        self.frames[LoginPage].update_on_incorrect_login_data()

    def update_on_no_server_connection(self):
        self.show_frame(StartPage)
        self.frames[StartPage].update_on_no_server_connection()

    def pass_login_password(self,login,password):
        self.client_controller.write_system_variable("login",login)
        self.client_controller.write_system_variable("password",password)


    def signal_client_controller_message(self,message):
        self.client_controller.write_system_message(message)

    def update_on_log_out(self):
        self.client_controller.log_out()
        self.show_frame(LoginPage)




class LoginPage(tk.Frame):
    def __init__(self,parent,controller):
        self.controller = controller
        tk.Frame.__init__(self,parent)
        self.failed_tries= 0
        self.labels = []

        self.label = tk.Label(self, text="Login page")
        self.label.grid(row=0, column=5)

        self.sendbutton = tk.Button(self, text="Login", width=10, height=2, command =self.login_action)
        self.sendbutton.grid(row=5, column=5)

        self.label2 = tk.Label(self, text="Username")
        self.label2.grid(row=3, column=5)

        self.login = tk.Entry(self,width=20)
        self.login.grid(row=3, column=6)

        self.label3 = tk.Label(self, text="Password")
        self.label3.grid(row=3, column=7)

        self.password = tk.Entry(self, width=20)
        self.password.grid(row=3, column=8)


    def update_on_incorrect_login_data(self):
        self.clear()
        self.failed_tries+=1

        if self.failed_tries <3:
            label = tk.Label(self, text="Incorrect login or password")
        else:
            label = tk.Label(self, text="Incorrect login or password \n try to contacct the administrator")

        self.labels.append(label)
        label.grid(row=9,column=5,columnspan=3)

    def send_login(self):
        print(f"sending login:{self.login.get(),self.password.get()}")

        self.controller.pass_login_password(self.login.get(),self.password.get())
        self.controller.signal_client_controller_message("login")

    def login_action(self):
        self.clear()

        if len(self.login.get())<1 or len(self.password.get())<1:
            self.update_on_missing_login_password()
        else:
            self.send_login()


    def update_on_missing_login_password(self):
        label = tk.Label(self,text="Both fields have to be filled")
        self.labels.append(label)
        label.grid(row=3, column=10, columnspan=3)

    def clear(self):
        for widget in self.labels:
            widget.destroy()





class StartPage(tk.Frame):
    def __init__(self,parent, controller):
        tk.Frame.__init__(self,parent)
        self.controller = controller
        self.label = tk.Label(self, text="Welcome page")
        self.label.grid(row=0, column=3)
        self.labels = []


        label_wait_to_connect = tk.Label(self,text="Waiting for connection...")
        label_wait_to_connect.grid(row=2, column=8)
        self.labels.append(label_wait_to_connect)

    def update_on_connected_to_server(self):
        self.clear()
        label = tk.Label(self,text=f"Connected \n Waiting for server...")
        label.grid(row=2,column = 4)
        self.labels.append(label)

    def update_on_no_server_connection(self):
        self.clear()
        label = tk.Label(self, text=f"Lost connection to server: server was taken down")
        label.grid(row=2, column=2)
        self.labels.append(label)

        reconnect_button = tk.Button(self,text="Reconnect",command=self.singal_reconnect)
        reconnect_button.grid(row =2 ,column =3)
        self.labels.append(reconnect_button)


    def clear(self):
        for widget in self.labels:
            widget.destroy()

    def singal_reconnect(self):
        self.controller.signal_client_controller_message("reconnect")







class Administration(tk.Frame):

    def __init__(self,parent, controller):
        tk.Frame.__init__(self,parent)
        self.parent = parent
        self.controller = controller
        self.account_list = [1,2,3]
        self.account_display_list = tk.Listbox(self)

        self.show_list()

        self.create_new_account_button = tk.Button(self,text="Create new account ", command=self.create_new_account_window)
        self.create_new_account_button.grid(row=1, column=3)

        self.edit_account_button = tk.Button(self, text="Edit ",command=self.edit_selected_account)
        self.edit_account_button.grid(row=1, column=4,)

        self.delete_account_button = tk.Button(self, text="Delete ",command=self.delete_selected_account)
        self.delete_account_button.grid(row=1,column=5)

        self.refresh_button = tk.Button(self, text="Refresh",command=self.refresh)
        self.refresh_button.grid(row=1,column=7)




        self.label = tk.Label(self,text='Administration')
        self.label.grid(row=0, column=0)



    def delete_selected_account(self):
        item_clicked = self.account_display_list.curselection()
        if item_clicked != ():
            pass

    def edit_selected_account(self):
        item_clicked = self.account_display_list.curselection()
        if item_clicked != ():
                print(self.account_display_list.get(item_clicked))


    def refresh(self):
        self.display_client_accounts_list()
        pass


    def show_list(self):

        self.account_display_list.grid(row=3, column=4, columnspan=3)
        self.account_display_list.insert(tk.END, "lulo")

    def create_new_account_window(self):
        PopUpAccount()

    def display_client_accounts_list(self):
        self.clear_list()
        for account in self.account_list:
            self.account_display_list.insert(tk.END,account)

    def clear_list(self):
        self.account_display_list.delete(0, tk.END)




        #self.account_name.insert(9, "a default value")


class MenuBar:

    def __init__(self,controller):
        self.controller = controller
        self.menubar = tk.Menu(controller)
        self.filemenu = tk.Menu(self.menubar)
        self.filemenu.add_command(label="LogOut",command=self.log_out_action)

        self.menubar.add_cascade(label = "Options",menu=self.filemenu)

        controller.config(menu=self.menubar)

    def show_menu(self):
        self.controller.config(menu=self.menubar)

    def hide_menu(self):
        self.controller.config(menu="")

    def log_out_action(self):
        self.controller.update_on_log_out()



class PopUpAccount():

    def __init__(self):
        self.labels = []
        self.popup = tk.Tk()
        self.popup.geometry(POPUP_SCREEN_SIZE)
        self.popup.wm_title("Create new account")
        label = tk.Label(self.popup, text="Create new account")
        label.grid(row=0, column=0)

        self.setup_popup()

        self.popup.mainloop()


    def setup_popup(self):

        name_label = tk.Label(self.popup, text='Name').grid(row=2, column=0)
        name = tk.Entry(self.popup).grid(row=2, column=1)

        username_label = tk.Label(self.popup, text='Username').grid(row=3, column=0)
        self.username = tk.Entry(self.popup).grid(row=3, column=1)

        password_label =  tk.Label(self.popup, text='Password').grid(row=5, column=0)
        self.password = tk.Entry(self.popup)
        self.password.grid(row=5, column=1)

        repeat_password_label = tk.Label(self.popup, text='Repeat password').grid(row=6, column=0)
        self.repeat_password = tk.Entry(self.popup)
        self.repeat_password.grid(row=6, column=1)

        email_label = tk.Label(self.popup, text='Email').grid(row=4, column=0)
        email = tk.Entry(self.popup).grid(row=4, column=1)

        account_button = tk.Button(self.popup, text="Create account ", command=self.create_account_button_action)
        account_button.grid(row=7, column=0)




    def create_account_button_action(self):

        if(self.password.get() != self.repeat_password.get()):
            self.passwords_does_not_match()
        else:
            self.data_ok_send_create_new_account()
            self.popup.destroy()

    def passwords_does_not_match(self):
        self.clear_labels()
        warning_label = tk.Label(self.popup,text="Password does not match")
        warning_label.grid(row=6, column=2)
        self.labels.append(warning_label)


    def data_ok_send_create_new_account(self):
        pass

    def clear_labels(self):
        for widget in self.labels:
            widget.destroy()

def clear_entry(entry_widget):
    entry_widget.delete(0,'end')