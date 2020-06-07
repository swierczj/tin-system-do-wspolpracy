import tkinter as tk


APP_NAME = 'Admin App'
SCREEN_SIZE = "400x300+700+200" # width_height right down

#amind class >> containter frame >> pages
class AdminApp(tk.Tk):

    def __init__(self):
        tk.Tk.__init__(self)
        self.setup()

        container = tk.Frame(self)
        container.pack(side="top",fill="both",expand=True)
        container.grid_rowconfigure(0,weight=1)
        container.grid_columnconfigure(0,weight=1)

        self.frames = {} #dict

        for F in (StartPage,Accounts,LoginPage):
            frame = F(container,self)
            self.frames[F] = frame
            frame.grid(row=0,column=0,sticky="nsew")

        self.show_frame(LoginPage)

    def show_frame(self,page):
        frame = self.frames[page]
        frame.tkraise()


    def setup(self):
        self.title(APP_NAME)
        self.geometry(SCREEN_SIZE)


class LoginPage(tk.Frame):
    def __init__(self,parent,controller):
        tk.Frame.__init__(self,parent)

        self.label = tk.Label(self, text="Login page")
        self.label.pack(pady=10, padx=10)


        self.label2 = tk.Label(self, text="Username")
        self.label2.pack(side=tk.LEFT)

        self.login = tk.Entry(self,width=20)
        self.login.pack(side=tk.LEFT,pady=100)

        self.label3 = tk.Label(self, text="Password")
        self.label3.pack(side=tk.LEFT,padx=10)
        self.password = tk.Entry(self, width=20)#,show='*'
        self.password.pack(side = tk.LEFT,pady=5)




class StartPage(tk.Frame):
    def __init__(self,parent, controller):
        tk.Frame.__init__(self,parent)
        label = tk.Label(self, text="Welcome page")
        label.pack(pady=10,padx=10)

        button = tk.Button(self,text="Manage accounts",
                               command=lambda: controller.show_frame(Accounts))
        button.pack()

class Accounts(tk.Frame):


    def __init__(self,parent, controller):
        tk.Frame.__init__(self,parent)

        self.label = tk.Label(self,text = 'Accounts')
        self.label.pack(pady=20,padx=20)

        self.account_name = tk.Entry(self,width=30)
        self.account_name.pack()



        button1 = tk.Button(self,text = "Back to welcome page",
                            command = lambda:controller.show_frame(StartPage))
        button1.pack()

        button2 = tk.Button(self, text="You typed:",
                            command=self.response)
        button2.pack()


    def response(self):
        txt = self.account_name.get()
        print(txt)
        self.label.configure(text=txt)
        self.account_name.delete(7, tk.END)

        #self.account_name.insert(9, "a default value")




adminapp = AdminApp()
adminapp.mainloop()