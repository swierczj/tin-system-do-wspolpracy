package client;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import java.io.Serializable;
import java.util.Base64;

public class Encryption implements Serializable{

    private static SecretKey secretKey;
    private static byte[] key;


    public static void setKey( String myKey ){
        secretKey = new SecretKeySpec( myKey.getBytes(), "AES" );
    }

    public static String encrypt( String strToEncrypt, String secret ){
        try{
            setKey( secret );
            Cipher cipher = Cipher.getInstance( "AES/ECB/PKCS5Padding" );
            cipher.init( Cipher.ENCRYPT_MODE, secretKey );
            return Base64.getEncoder().encodeToString( cipher.doFinal( strToEncrypt.getBytes( "UTF-8" ) ) );
        }catch( Exception e ){
            System.out.println( "Error while encrypting: " + e.toString() );
        }
        return null;
    }

    public static String decrypt( String strToDecrypt, String secret ){
        try{
            setKey( secret );
            Cipher cipher = Cipher.getInstance( "AES/ECB/PKCS5PADDING" );
            cipher.init( Cipher.DECRYPT_MODE, secretKey );
            return new String( cipher.doFinal( Base64.getDecoder().decode( strToDecrypt ) ) );
        }catch( Exception e ){
            System.out.println( "Error while decrypting: " + e.toString() );
        }
        return null;
    }
}
