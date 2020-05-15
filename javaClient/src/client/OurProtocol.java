package client;

public class OurProtocol {
    private String message;
    private String login;
    private String password;
    private int statement;
    private int keyType;
    private String key;


    public OurProtocol(String login, String password){
        setLoginAndPassword(login, password);
    }

    public OurProtocol(String message, int type) {
        switch (type) {
            case 0:
                if (loginAndPassword( message ) != 0) {
                    login = null;
                    password = null;
                }
            case 1:
                statement = Integer.parseInt( message );
            case 2:
                this.message = message;
            case 3:
                if (keyAndType( message ) != 0) {
                    key = null;
                    keyType = -1;
                }

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

    private int loginAndPassword( String message ) {
        String[] splitted = this.message.split("\n");
        if (splitted.length != 2)
            return -1;
        login = splitted[0];
        password = splitted[1];
        return 0;
    }

    private int keyAndType( String message ) {
        String[] splitted = this.message.split("\n");
        if (splitted.length != 2)
            return -1;
        keyType = Integer.parseInt( splitted[0] );
        key = splitted[1];
        return 0;
    }

    public int getStatement(){
        return statement;
    }
    public int getKeyType(){
        return keyType;
    }
    public String getKey(){ return key; }
}