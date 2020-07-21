package Boneh_Lynn_Shacham_scheme;


import java.math.BigInteger;
import java.security.*;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.Pairing;

public class MessageSign {
	private static String hashFunction = "SHA-512";
	private static String hashedMessage;
	private static Element hashMappedToGroup;
	private static Element signature;
	
	static void hashMessage(String message) {
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
					
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	static void mapHashToGroup() {
		// a mapping from the hashed string to an element of the group
		//hashMappedToGroup = new Setup().getPairing().getG1().newElement().setFromHash(hashedMessage.getBytes(), 0, hashedMessage.length());
		Pairing pairing = Setup.getPairing();
		Field G1 = pairing.getG1();
		Element ele = G1.newElement();
		hashMappedToGroup = ele.setFromHash(hashedMessage.getBytes(), 0, hashedMessage.length());
		System.out.println("Hashed message maps to group element: " + hashMappedToGroup);
	}
	
	static void computeSignature() {
		
		//signature = hashMappedToGroup.powZn(new KeyGeneration().getPrivateKey());
		Element privateKey = KeyGeneration.getPrivateKey();
		signature = hashMappedToGroup.powZn(privateKey);
		System.out.println("Signature: " + signature);
	}
	
	static Element getSignature() {return signature;}
	
	static void initialiseMessageSigning(String message) {
		hashMessage(message);
		mapHashToGroup();
		computeSignature();
	}
	
}
