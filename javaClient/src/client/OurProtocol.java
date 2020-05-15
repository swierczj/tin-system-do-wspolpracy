package client;

public class OurProtocol {
    private String message;
    private String login;
    private String password;
    private int statement;


    public OurProtocol(String login, String password){
        setLoginAndPassword(login, password);
    }

    public OurProtocol(String message, int type) {
        this.message = message;
        switch (type) {
            case 0:
                if (loginAndPassword() != 0) {
                    login = null;
                    password = null;
                }
            case 1:
                statement = Integer.parseInt( message );
        }
    }

    public OurProtocol (int statement) {
        Integer toString = statement;
        message = toString.toString();

        login = null;
        password = null;
    }

    public OurProtocol (String edit) {
        message = edit;
        login = null;
        password = null;
    }

    public String getLogin() {

        return login;
    }

    public String getMessage() {
        return message;
    }

    public String getPassword() {
        return password;
    }

    private void setLoginAndPassword(String login, String password) {
        this.message = login + "\n" + password;
    }

    private int loginAndPassword() {
        String splitted[] = message.split("\n");
        if (splitted.length != 2)
            return -1;
        login = splitted[0];
        password = splitted[1];
        return 0;
    }

    public int getStatement(){
        return statement;
    }
}
