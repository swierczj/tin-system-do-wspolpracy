package client;

public class OurProtocol {
    private String message;
    private String login;
    private String password;


    public OurProtocol(String login, String password){
        setLoginAndPassword(login, password);
    }

    public OurProtocol(String message, int state) {
        this.message = message;
        if (state == 1) {
            if (loginAndPassword() != 0) {
                login = null;
                password = null;
            }
        }
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
}
