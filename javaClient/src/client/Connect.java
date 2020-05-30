package client;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.io.IOException;

public class Connect{
    @FXML public void initialize(){}

    @FXML protected void setDefault( ActionEvent event ){
        if( defaultCheckBox.isSelected() ){
            ipField.setText( defaultIp );
            ipField.setDisable( true );
            portField.setText( defaultPort );
            portField.setDisable( true );
        } else{
            ipField.setText( "" );
            ipField.setDisable( false );
            portField.setText( "" );
            portField.setDisable( false );
        }
    }

    @FXML protected void setLoginData( ActionEvent event ) throws IOException{
        ip = ipField.getText();
        try{
            port = Integer.parseInt( portField.getText() );
        } catch( NumberFormatException e ){
            port = 0;
        }
        if( createClient() == 0 )     // hide window
            loginButton.getScene().getWindow().hide();
    }

    private int createClient() throws IOException{
        if( !connected ){
            client = new Client( ip, port );
            if( client.connect() != 0 ){
                displayError( "Cannot connect to " + ip + ":" + portField.getText() );
                return -1;
            }
            connected = true;
            System.out.print( "connect" );
        }
        client.setLoginData( loginField.getText(), passwordField.getText() );
        if( client.login() != 0 ){
            displayError( "Login or password incorrect." );
            return -2;
        }
        return 0;
    }

    private void displayError( String errMsg ){
        Stage stage = new Stage();
        stage.setTitle( "Error" );
        stage.setScene( new Scene( new Label( errMsg ), 300, 100 ) );
        stage.initModality( Modality.APPLICATION_MODAL );
        stage.showAndWait();
    }

    @FXML private TextField loginField;
    @FXML private TextField ipField;
    @FXML private TextField portField;
    @FXML private PasswordField passwordField;
    @FXML private CheckBox defaultCheckBox;
    @FXML private Button loginButton;
    private String defaultIp = "185.20.175.81";
    private String defaultPort = "54000";
    private String ip;
    private int port;
    private boolean connected = false;

    private Client client;
}
