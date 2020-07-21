package Boneh_Lynn_Shacham_scheme;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import it.unisa.dia.gas.jpbc.Element;

public class Verification {
	
	// on receiving some message, verify if the signature is true
	private static String hashedMessage;
	private static String hashFunction = "SHA-512";
	
	static void hashMessage(String message) {
		try {
			MessageDigest message_digest = MessageDigest.getInstance(hashFunction);
			byte[] inputDigest = message_digest.digest(message.getBytes());
			BigInteger inputDigestBigInteger = new BigInteger(1, inputDigest);
			
			String hashText = inputDigestBigInteger.toString(16);
			while(hashText.length() < 32)
					hashText = "0" + hashText;
			
			hashedMessage = hashText;
			System.out.println("--------- Verification ------------");					
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	static String hashMessageReturn(String message) {
		try {
			MessageDigest message_digest = MessageDigest.getInstance(hashFunction);
			byte[] inputDigest = message_digest.digest(message.getBytes());
			BigInteger inputDigestBigInteger = new BigInteger(1, inputDigest);
			
			String hashText = inputDigestBigInteger.toString(16);
			while(hashText.length() < 32)
					hashText = "0" + hashText;
			
			hashedMessage = hashText;
			System.out.println("\n--------- Signature ------------");
			System.out.println("Hashed message: " + hashedMessage);
			return hashedMessage;
					
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		
	}
	
	static void verifySignature(String text) {
		hashMessage(text);
		Element h = Setup.getPairing().getG1().newElement().setFromHash(hashedMessage.getBytes(), 0, hashedMessage.length());
		Element left = Setup.getPairing().pairing(Setup.getGenerator(), MessageSign.getSignature());
		Element right = Setup.getPairing().pairing(h, KeyGeneration.getPublicKey());
		// this is just luck that both (h, KeyGeneration.getPublicKey()) and (KeyGeneration.getPublicKey(), h) give true
		System.out.println("Verification result: " + left.isEqual(right));
		
		Element A = Setup.getGenerator();
		Element m = Setup.getPairing().getZr().newRandomElement();		
		String hash = hashMessageReturn("abc");
		Element B = Setup.getPairing().getG1().newElement().setFromHash(hash.getBytes(), 0, hash.length());
		left = Setup.getPairing().pairing(A.powZn(m), B);
		right = Setup.getPairing().pairing(B.powZn(m), A); // inverted
		System.out.println(left.isEqual(right));
	}
}
