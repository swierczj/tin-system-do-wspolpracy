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
        Character c = new Character();
        c.append( 0, 0 );
        text.add( c );
        addedBuffer = new ArrayList<>();
        deletedBuffer = new ArrayList<>();
        textArea.setOnKeyTyped( new EventHandler< KeyEvent >(){
            @Override
            public void handle( KeyEvent keyEvent ){
                proceedKeyTyped( keyEvent );
            }
        } );
    }

    public String getChanges(){
        StringBuilder str = new StringBuilder();
        for( Character c : addedBuffer )
            str.append( c.toString() ).append( ( char )CHAR_SPLITTER_ASCII_CODE );
        str.append( ( char )BUFFER_SPLITTER_ASCII_CODE );
        for( Character c : deletedBuffer )
            str.append( c.toString() ).append( ( char )CHAR_SPLITTER_ASCII_CODE );
        return str.toString();
    }

    private void proceedKeyTyped( KeyEvent keyEvent ){
        int caretPos = textArea.getCaretPosition();
        if( keyEvent.getCharacter().equals( String.valueOf( ( char )( 127 ) ) ) ){          // Cannot delete from end of string ( still remember of 1 additional element )
            if( caretPos < text.size() - 1 ){
                int charPos = caretPos + 1;      // delete what was before caret so what is on position
                Character c = text.remove( charPos );               // of actual caret ( +1 bc we star count signs from 1 )
                deletedBuffer.add( c );
                System.out.print( "\ndelete: " + c.toString() );
            }
        } else if( keyEvent.getCharacter().equals( "\b" ) ){   // when backspace
            if( textArea.getText().length() < text.size() - 1 ){    // cannot delete from begin of string
                int charPos = caretPos + 1;      // delete what was before caret so what is on position
                Character c = text.remove( charPos );               // of actual caret ( +1 bc we star count signs from 1 )
                deletedBuffer.add( c );
                System.out.print( "\ndelete: " + c.toString() );
            }
        } else{
            int charPos = caretPos - 1;      // append on previous caret pos, so on actPos - 1
            String key = keyEvent.getCharacter();               // numerating from 1 is applied in getChar() algorithm
            Character c = getChar( key, charPos );
            text.add( charPos + 1, c );
            addedBuffer.add( c );
            System.out.print( "\n" + c.toString() );
        }
    }

    @FXML private void displayChangesBuffer( ActionEvent event ){       // TODO only for check, to remove from final version
        System.out.print( getChanges() );
    }

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
                c.position.addAll( next.position );
                c.append( c.pop() - 1, creatorId );
                if( c.position.get( c.position.size() - 1 ).pos < 1 )     // we have to add 1 on the end
                    c.append( 1, creatorId );
                return c;
            }
            // CASE 2
            // text[ index + 1 ][ i ] doesn't exist
            // Should be only when 'on end insertion'
            // New char's position will be head of previous incremented by 1
            else if( next.position.size() <= i && index >= text.size() - 1 ){
                c.append( prev.position.get( 0 ).pos + 1, creatorId );
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
                    c.append( c.pop() + 1, creatorId );
                // CASE 3.2
                // Previous position's 'i' element is last element
                // We take previous element and append 1
                else
                    c.append( 1, creatorId );
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
    private List< Character > addedBuffer;
    private List< Character > deletedBuffer;
    private final int CHAR_SPLITTER_ASCII_CODE = 29;   // Group separator ( to split characters in message )
    private final int BUFFER_SPLITTER_ASCII_CODE = 28; // File sparator ( to split added chars from deleted )

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
