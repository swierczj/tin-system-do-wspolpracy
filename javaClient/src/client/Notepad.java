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
        charBuffer = new ArrayList<>();
        textArea.setOnKeyTyped( new EventHandler< KeyEvent >(){
            @Override
            public void handle( KeyEvent keyEvent ){
                proceedKeyTyped( keyEvent );
            }
        } );
    }

    private void proceedKeyTyped( KeyEvent keyEvent ){
        int caretPos = textArea.getCaretPosition() - 1;
        String key = keyEvent.getCharacter();
        Character c = getChar( key, caretPos );
        text.add( caretPos + 1, c );
        charBuffer.add( c );
        System.out.print( c.toString() );
    }

    @FXML private void displayChangesBuffer( ActionEvent event ){
        for( Character c : charBuffer )
            System.out.print( c.toString() + "\n");
    }

    // alg without deleting chars   // TODO can be deleted i think
    // iter 0 when calling outside
//    private Character getChar( String c, int index, int iter ){       // index - previous char, index + 1 - next char
//        Character character = new Character();
//        character.c = c.charAt( 0 );
//        if( index < text.size() - 1 ){     // not on the end, bc there is no next char
//            if( index != 0 ){       //inaczej sie wyjebie
//                if( text.get( index ).position.size() > iter ){     // to avid nullptr
//                    // CASE 1: 'iter' pos id is different
//                    if( text.get( index ).position.get( iter ) != text.get( index + 1 ).position.get( iter ) ){
//                        character.position.addAll( text.get( index ).position );    // takes previous's char position
//                        character.append( character.pop() + 1, creatorId ); // changes last position id
//                    }else{  // CASE 2: 'iter' pos id is equal
//                        getChar( c, index, iter + 1 );
//                    }
//                }else{     // CASE 3: 'iter' pos id doesn't exist in previous char
//                    character.position.addAll( text.get( index + 1 ).position );    // we take next pos and replace end with 0
//                    character.pop();
//                    character.append( 0, creatorId );
//                    character.append( 1, creatorId );
//                }
//            }else{     // CASE 4: begin of string
//                character.position.addAll( text.get( index + 1 ).position );    // we take next pos and replace end with 0
//                character.pop();
//                character.append( 0, creatorId );
//                character.append( 1, creatorId );
//            }
//        } else{     //CASE 5: end of string
//            character.position.addAll( text.get( index ).position );
//            character.append( character.pop() + 1, creatorId );
//        }
//        return character;
//    }

    // text[ 0 ] = 0, invisible, only for 'on begin insertions' case
    // text[ index ] is previous char
    // text[ index + 1 ] is next char
    private Character getChar( String s, int index ){
        Character c = new Character();
        c.c = s.charAt( 0 );        // New character struct
        Character prev = text.get( index );
        Character next = text.get( index + 1 );

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
            }
            // CASE 2
            // text[ index + 1 ][ i ] doesn't exist
            // Should be only when 'on begin insertion'
            // New char's position will be head of previous incremented by 1
            else if( next.position.size() <= i && index >= text.size() - 1 ){
                c.append( prev.position.get( 0 ).pos, creatorId );
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
                    c.append( c.pop() - 1, creatorId );
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
