package client;

public class Message{
    private String message;
    private String login;
    private String password;
    private int statement;
    private int keyType;
    private int clientId;
    private String key;
    private boolean correct = true;


    public Message( String login, String password){
        setLoginAndPassword(login, password);
    }

    public Message( String message, int type) {
        if( message == null ){
            correct = false;
            return;
        }
        switch (type) {
            case 0:
                if (loginAndPassword() != 0) {
                    login = null;
                    password = null;
                }
                break;
            case 1:
                statement = Integer.parseInt( message.trim() );
                break;
            case 2:
            case 5:
                this.message = message;
                break;
            case 3:
                if (keyAndType() != 0) {
                    key = null;
                    keyType = -1;
                }
                break;
            case 4:
                this.clientId = Integer.parseInt( message.trim() );
                break;
        }
    }

    public Message( int statement) {
        message = Integer.toString( statement );
        login = null;
        password = null;
    }

    public Message( String edit) {
        message = edit;
        login = null;
        password = null;
    }

    public String getLogin() { return login; }
    public String getMessage() { return message; }
    public String getPassword() { return password; }
    public int getClientId() { return clientId; }
    public int getStatement(){ return statement; }
    public int getKeyType(){ return keyType; }
    public String getKey(){ return key; }
    public boolean isCorrect(){ return correct; }

    private void setLoginAndPassword(String login, String password) {
        this.message = login + "\n" + password;
    }

    private int loginAndPassword() {
        String[] splitted = this.message.split("\n");
        if (splitted.length != 2)
            return -1;
        login = splitted[0];
        password = splitted[1];
        return 0;
    }

    private int keyAndType() {
        String[] splitted = this.message.split("\n");
        if (splitted.length != 2)
            return -1;
        keyType = Integer.parseInt( splitted[0] );
        key = splitted[1];
        return 0;
    }


}