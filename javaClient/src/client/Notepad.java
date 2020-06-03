package client;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.MenuBar;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.awt.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class Notepad{
    @FXML public void initialize(){
        text = new LinkedList<>();
        Character c = new Character();
        c.append( 0, 0 );
        text.add( c );
        addedBuffer = new ArrayList<>();
        deletedBuffer = new ArrayList<>();
        setOnDrag();
        textArea.setOnKeyTyped( this::proceedKeyTyped );
    }
    public void setClient( Client client ){ this.client = client; }

    private void setOnDrag(){
        dragPane.setOnMousePressed( mouseEvent -> {
            x = mouseEvent.getSceneX();
            y = mouseEvent.getSceneY();
        } );

        dragPane.setOnMouseDragged( mouseEvent -> {
            Stage stage = ( Stage ) dragPane.getScene().getWindow();
            stage.setX( mouseEvent.getScreenX() - x );
            stage.setY( mouseEvent.getScreenY() - y );
            stage.setOpacity( 0.9 );
        } );

        dragPane.setOnMouseReleased( mouseEvent -> dragPane.getScene().getWindow().setOpacity( 1.0 ) );
    }

    public String getChanges(){
        StringBuilder str = new StringBuilder();
        for( Character c : deletedBuffer )
            str.append( c.toString() ).append( ( char )CHAR_SPLITTER_ASCII_CODE );
        str.append( ( char )BUFFER_SPLITTER_ASCII_CODE );
        for( Character c : addedBuffer )
            str.append( c.toString() ).append( ( char )CHAR_SPLITTER_ASCII_CODE );
        addedBuffer.clear();
        deletedBuffer.clear();
        return str.toString();
    }

    public String getTextFromCodedInMemory(){
        StringBuilder str = new StringBuilder();
        for( int i = 1; i < text.size(); ++ i )
            str.append( text.get( i ).c );
        return str.toString();
    }

    // Function applies incoming changes (from server)
    public void applyChanges( String changes ){
        int caretPos = textArea.getCaretPosition();
        String[] temp = changes.split( String.valueOf( ( char )BUFFER_SPLITTER_ASCII_CODE ), 0 );
        String[] deletedChars = temp[ 0 ].split( String.valueOf( ( char )CHAR_SPLITTER_ASCII_CODE ), 0 );
        String[] addedChars = temp[ 1 ].split( String.valueOf( ( char )CHAR_SPLITTER_ASCII_CODE ), 0 );
        for( String c : deletedChars )
            removeChar( toCharacter( c ), caretPos );
        for( String c : addedChars )
            addChar( toCharacter( c ), caretPos );
        textArea.setText( getTextFromCodedInMemory() );
    }

    private void removeChar( Character c, int caretPos ){
        for( int i = 1; i < text.size(); ++i ){
            if( compare( c, text.get( i ) ) == 0 ){
                text.remove( i );
                if( i <= caretPos )
                    textArea.positionCaret( caretPos - 1 );
            }
        }
    }

    private void addChar( Character c, int caretPos ){
        for( int i = 1; i < text.size(); ++i ){     // first char is 0
            int temp = compare( c, text.get( i ) );
            if( temp == 1 );    // faster loop, it's almost always true
            else if( temp == -1 ){
                text.add( i, c );       // add before greater char
                if( i <= caretPos )
                    textArea.positionCaret( caretPos + 1 );
                return;
            } else return;      // compare returned 0 or -2
        }
    }

    private void proceedKeyTyped( KeyEvent keyEvent ){
        int caretPos = textArea.getCaretPosition();
        String event = keyEvent.getCharacter();
        int diff = text.size() - textArea.getLength() - 1;      // How many chars deleted
        if( event.equals( String.valueOf( ( char )( 127 ) ) ) || event.equals( "\b" ) ){          // Cannot delete from end of string ( still remember of 1 additional element )
            int charPos = caretPos + 1;      // delete what was before caret so what is on position
            removeMultipleChars( diff, charPos );
        } else if( ( int )event.charAt( 0 ) >= 32 || ( int )event.charAt( 0 ) == 9 || ( int )event.charAt( 0 ) == 13  ){             // only for printable chars
            int charPos = caretPos - 1;                 // append on previous caret pos, so on actPos - 1
            removeMultipleChars( diff + 1, caretPos );        // numerating from 1 is applied in getChar() algorithm
            Character c = getChar( event, charPos );
            text.add( charPos + 1, c );
            addedBuffer.add( c );
            System.out.print( "\n" + c.toString() );
        } else{
            //displayError( "Operation forbidden\nIt will be undone." );
            textArea.setText( getTextFromCodedInMemory() );
            textArea.positionCaret( prevCaretPos );
        }
        prevCaretPos = caretPos;
    }

    public void displayError( String errMsg ){
        Stage stage = new Stage();
        stage.setTitle( "Error" );
        Label label = new Label( errMsg );
        label.setAlignment( Pos.CENTER );
        stage.setScene( new Scene( label, 300, 100 ) );
        stage.initModality( Modality.APPLICATION_MODAL );
        stage.showAndWait();
    }

    private void removeMultipleChars( int howMany, int position ){
        for( int i = position; i < howMany + position; ++ i ){
            Character c = text.remove( position );               // of actual caret ( +1 bc we star count signs from 1 )
            deletedBuffer.add( c );
            System.out.print( "\ndelete: " + c.toString() );
        }
    }

    @FXML private void displayChangesBuffer(){       // TODO only for check, to remove from final version
        System.out.print( getChanges() );
    }

    @FXML private void setPrevCaretPos() { prevCaretPos = textArea.getCaretPosition(); }

    @FXML private void save() throws FileNotFoundException{
        if( fileName.equals( "Untitled.txt" ) )
            saveAs();
        else{
            File f = new File( fileName );
            save( f );
        }
    }

    private void save( File file ) throws FileNotFoundException{
        fileName = file.getPath();
        ( ( Stage )textArea.getScene().getWindow() ).setTitle( "Notepad - " + file.getName() );
        PrintWriter pw = new PrintWriter( file );
        pw.print( getTextFromCodedInMemory() );
        pw.close();
    }

    @FXML private void saveAs() throws FileNotFoundException{
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle( "Save as" );
        fileChooser.setInitialFileName( new File( fileName ).getName() );
        if( new File( fileName ).exists() )
            fileChooser.setInitialDirectory( new File( fileName ).getParentFile() );
        FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter( "Text files (*.txt)", "*.txt" );
        fileChooser.getExtensionFilters().add( extFilter );
        File file = fileChooser.showSaveDialog( textArea.getScene().getWindow() );
        if( file != null ) save( file );
    }

    @FXML private void openFromServer(){
        client.fileListRequest();
    }

    @FXML public void quit(){
        textArea.getScene().getWindow().hide();
        client.quit();
        Platform.exit();
        System.exit( 0 );
    }

    @FXML private void reconnect() throws IOException{
        if( client.connect() != 0 )
            displayError( "Cannot connect.\nPlease try again or restart application." );
    }

    @FXML private void goToGitHub(){
        try{
            Desktop.getDesktop().browse( new URL( "https://github.com/swierczj/tin-system-do-wspolpracy" ).toURI() );
        }catch( IOException | URISyntaxException e ){
            displayError( "Cannot open your browser.\nPlease visit our GitHub repo from that URL:\nhttps://github.com/swierczj/tin-system-do-wspolpracy" );
        }
    }

    public String fileSelect( String[] names ){
        Stage stage = new Stage();
        stage.setTitle( "Select file to edit" );
        ListView< String > list = new ListView< String >( FXCollections.observableArrayList( names ) );
        list.setPrefSize( 640, 480 );
        list.setEditable( false );
        stage.setScene( new Scene( list, 640, 480 ) );
        stage.initModality( Modality.APPLICATION_MODAL );
        list.getSelectionModel().selectedItemProperty().addListener( new ChangeListener< String >(){
            @Override
            public void changed( ObservableValue< ? extends String > observableValue, String s, String t1 ){
                serverFile = t1;
            }
        } );
        stage.showAndWait();
        System.out.print( serverFile );
        return serverFile;
    }

    // Dzial funkcji powalonych, lepiej do nich nie wracac
    // text[ 0 ] = 0, invisible, only for 'on begin insertions' case
    // text[ index ] is previous char
    // text[ index + 1 ] is next char
    private Character getChar( String s, int index ){       // TODO napisac dla roznych klientow ( porownywac id tworcy znaku )
        Character c = new Character();
        c.c = s.charAt( 0 );        // New character struct
        Character prev = text.get( index );
        Character next = new Character();
        if( text.size() - 1 >= index + 1 )      // prevent index out of bounds
            next = text.get( index + 1 );

        // Insertion between index and index+1
        // We compare next positions in loop
        for( int i = 0; ; ++i ){
            // CASE 1
            // text[ index ][ i ] doesn't exist
            // Position will be position of next char with decremented last element
            if( prev.position.size() <= i ){
                // CASE 666
                // Both whole positions are equal
                if( next.position.size() <= i ){
                    c.position.addAll( prev.position );
                    c.append( 1, clientId );
                    return c;
                }
                c.position.addAll( next.position );
                c.append( c.pop() - 1, clientId );
                if( c.position.get( c.position.size() - 1 ).pos < 1 )     // we have to add 1 on the end
                    c.append( 1, clientId );
                return c;
            }
            // CASE 2
            // text[ index + 1 ][ i ] doesn't exist
            // Should be only when 'on end insertion'
            // New char's position will be head of previous incremented by 1
            else if( next.position.size() <= i && index >= text.size() - 1 ){
                c.append( prev.position.get( 0 ).pos + 1, clientId );
                return c;
            }
            // CASE 3
            // positions[ i ] are different
            else if( prev.position.get( i ).pos != next.position.get( i ).pos ){
                c.position.addAll( prev.position );
                // CASE 3.1
                // Previous position has more elements than on i position
                // New char's position will be previous char's position with last element incremented
                if( prev.position.size() - 1 > i )
                    c.append( c.pop() + 1, clientId );
                // CASE 3.2
                // Previous position's 'i' element is last element
                // We take previous element and append 1
                else
                    c.append( 1, clientId );
                return c;
            }
            // CASE 4
            // positions[ i ] are equal
            // We compare next positions by continuing the for loop
        }
        // CASE 5
        // Sth fucked up
        // I would throw an exception but intelliJ has a problem because it's unreachable statement
        // that's pretty satisfying
    }

    private int compare( Character first, Character second ){    // 1<2 : -1, 1>2 : 1, 1=2: 0, same pos, diff chars : -2
        for( int i = 0; ; ++i ){
            if( first.position.size() <= i ){           // if some of pos( i ) don't exist
                if( second.position.size() <= i ){       // whole positions are equal, check creators Id
                    if( first.position.get( i - 1 ).creator > second.position.get( i - 1 ).creator )
                        return 1;
                    if(  first.position.get( i - 1 ).creator < second.position.get( i - 1 ).creator  )
                        return -1;
                    if( first.c == second.c ) return 0; // exactly equal
                    return -2;          // equal positions, different chars, sth fucked, we have to reject char
                }
                return -1;                  // 1 position has ended
            }
            if( second.position.size() <= i )
                return 1;                   // 2 position has ended
            if( first.position.get( i ).pos > second.position.get( i ).pos )
                return 1;                   // both pos( i ) exist, 1 is greater
            if( second.position.get( i ).pos > first.position.get( i ).pos )
                return -1;                  // both pos( i ) exist, 2 is greater
        }           // both pos( i ) are equal, next loop
        //if( first.c == second.c ) return 0;
        //return -2;
    }

    private Character toCharacter( String str ){
        Character c = new Character();
        c.c = str.charAt( 0 );
        String[] splitStr = str.substring( 1 ).split( "_", 0 );
        for( String s : splitStr ){
            String[] splitPair = s.split( "," );
            c.append( Integer.parseInt( splitPair[ 0 ] ), Integer.parseInt( splitPair[ 1 ] ) );
        }
        return c;
    }

    @FXML TextArea textArea;
    @FXML private MenuBar dragPane;
    private double x = 0, y = 0;
    private List< Character > text;
    private List< Character > addedBuffer;
    private List< Character > deletedBuffer;
    private final int CHAR_SPLITTER_ASCII_CODE = 29;   // Group separator ( to split characters in message )
    private final int BUFFER_SPLITTER_ASCII_CODE = 28; // File sparator ( to split added chars from deleted )
    private int prevCaretPos = 0;

    private class Character{
        public Character(){
            position = new ArrayList<>();
        }
        // Appends element on end of pos array
        public void append( int p, int c ){
            this.position.add( new pair( p, c ) );
        }

        public int pop(){
            int ret = this.position.get( this.position.size() - 1 ).pos;
            this.position.remove( this.position.size() - 1 );
            return ret;
        }

        // eg. A, (1,0), (2,0) coded like:
        // A1,0_2,0_
        public String toString(){
            StringBuilder str = new StringBuilder();
            str.append( this.c );
            for( Character.pair pair : this.position )
                str.append( pair.pos ).append( "," ).append( pair.creator ).append( "_" );
            return str.toString();
        }

        public char c;
        public List< pair > position;      // position CRDT
        private class pair{
            pair( int p, int c ){ pos = p; creator = c; }
            public int pos;         // sign position
            public int creator;     // Who created sign
        }
    }

    private String serverFile = "";                 // server file name
    private String fileName = "Untitled.txt";       // local name
    private int clientId = 7777;
    private Client client;

    public void setClientId( int clientId ){
        this.clientId = clientId;
    }
}
