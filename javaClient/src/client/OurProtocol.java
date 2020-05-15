package client;

public class OurProtocol {
    private String message;
    private String login;
    private String password;


    public OurProtocol(String message) {
        this.message = message;
        login = "";
        password = "";
    }

    private void loginAndPassword() {
       String splitted[] = message.split("\n");
       login = splitted[0];
       password = splitted[1];
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

}
