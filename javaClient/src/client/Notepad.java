package client;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyEvent;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class Notepad{
    @FXML public void initialize(){
        text = new LinkedList<>();
        charBuffer = new ArrayList<>();
        textArea.setOnKeyTyped( new EventHandler< KeyEvent >(){
            @Override
            public void handle( KeyEvent keyEvent ){
                proceedKeyTyped( keyEvent );
            }
        } );
    }

    private void proceedKeyTyped( KeyEvent keyEvent ){
        int caretPos = textArea.getCaretPosition();
        String key = keyEvent.getCharacter();
        Character c = getChar( key, caretPos, 0 );
        text.add( caretPos, c );
        charBuffer.add( c );
    }

    @FXML private void displayChangesBuffer( ActionEvent event ){
        for( Character c : charBuffer )
            System.out.print( c.toString() + "\n");
    }

    // alg without deleting chars
    // iter 0 when calling outside
    private Character getChar( String c, int index, int iter ){       // index - previous char, index + 1 - next char
        Character character = new Character();
        character.c = c.charAt( 0 );
        if( index != 0 ){       //inaczej sie wyjebie
            if( text.get( index ).position.size() > iter ){     // to avid nullptr
                // CASE 1: 'iter' pos id is different
                if( text.get( index ).position.get( iter ) != text.get( index + 1 ).position.get( iter ) ){
                    character.position = text.get( index ).position;    // takes previous's char position
                    character.append( character.pop() + 1, creatorId ); // changes last position id
                }else{  // CASE 2: 'iter' pos id is equal
                    getChar( c, index, iter );
                }
            } else{     // CASE 3: 'iter' pos id doesn't exist in previous char
                character.position = text.get( index + 1 ).position;    // we take next pos and replace end with 0
                character.pop();
                character.append( 0, creatorId );
                character.append( 1, creatorId );
            }
        } else{     // CASE 4: begin of string
            character.position = text.get( index + 1 ).position;    // we take next pos and replace end with 0
            character.pop();
            character.append( 0, creatorId );
            character.append( 1, creatorId );
        }
        return character;
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

    private List< Character > text;
    private List< Character > charBuffer;

    private class Character{
        Character(){
            position = new ArrayList<>();
        }
        char c;
        List< pair > position;      // position CRDT
        private class pair{
            pair( int p, int c ){ pos = p; creator = c; }
            public int pos;         // sign position
            public int creator;     // Who created sign
        }
        // Appends element on end of pos array
        public void append( int p, int c ){ this.position.add( new pair( p, c ) ); }
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
    }

    private int creatorId = 0;      // TODO
}
