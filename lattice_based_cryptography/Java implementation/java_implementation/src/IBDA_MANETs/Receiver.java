package IBDA_MANETs;

import java.math.BigInteger;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Pairing;

public class Receiver {
	private static Element privateKey;
	private static String identity;
	
	private static Element publicKey; // Qid = H1(ID) 
	
	private static void initialisePublicKey() {
		// map identity to some element in G1 using H1(.)
		publicKey = PKG.getHashByH1(identity);
		System.out.println("Public key Qid: " + publicKey);
	}
	
	private static boolean verifyPrivateKey() {
		// Verification: e(Di, P) = e(Qid, Po)
		// P = primitive root of G1 in PKG
		// Po = master public key in PKG
		// Di = generated private key from PKG
		// Qid = hash of the identity of the sender
		Element P = PKG.getG1Generator();
		Element Po = PKG.getMasterPublicKey();
		Pairing pairing = PKG.getPairing();
		Element left = pairing.pairing(publicKey, Po);
		Element right = pairing.pairing(privateKey=PKG.computePrivateKey(publicKey), P);
		return (left.isEqual(right));
	}
	
	static void initialiseReceiver(String identity) {
		Receiver.identity = identity;
		System.out.println("---------------------------------- Receiver initialisation--------------------------");
		System.out.println("Private key requested from PKG");
		initialisePublicKey();
		System.out.println("Private key received");
		System.out.println("Private key verification ( e(Di, P) = e(Qid, Po) ): " + verifyPrivateKey());
		System.out.println("---------------------------------- Receiver initialisation over--------------------------");
	}
	
	static Element getPublicKey() {return publicKey;}
	
	private static String xorHex(String a, String b) {
	    // TODO: Validation
		
		int hashLength = a.length();
		int messageLength = b.length();
		int diff = hashLength - messageLength;
		
		if(diff > 0) {
			for(int i = 0; i < diff; i++)
				b = '\0' + b;
		}
		
		else if(diff < 0) {
			diff = -1* diff;
			for(int i = 0; i < diff; i++)
				a = '\0' + a;
		}
		
		System.out.println("Concerned strings are of equal length? -->" + (a.length() == b.length()));
		
	    char[] chars = new char[a.length()];
	    for (int i = 0; i < chars.length; i++) {
	        chars[i] = toHex(fromHex(a.charAt(i)) ^ fromHex(b.charAt(i)));
	    }
	    return new String(chars);
	}

	private static int fromHex(char c) {
		if (c >= '0' && c <= '9') {
		        return c - '0';
		    }
		if (c >= 'A' && c <= 'F') {
		        return c - 'A' + 10;
		    }
		if (c >= 'a' && c <= 'f') {
		        return c - 'a' + 10;
		    }
	    return '\0';
	}

	private static char toHex(int nybble) {
	    if (nybble < 0 || nybble > 15) {
	        throw new IllegalArgumentException();
	    }
	    return "0123456789ABCDEF".charAt(nybble);
	}
	
	static void receiveFromSender() {
		String message = Sender.getMessage();
		Element PSI_sender = Sender.getPSI_sender();
		String XOR_output = Sender.getXOROutput();
		Pairing pairing = PKG.getPairing();
		Element G_receiver = pairing.pairing(PSI_sender, privateKey=PKG.computePrivateKey(publicKey));
		String hash = PKG.getHashByH2(G_receiver);
		System.out.println("H2 (G_receiver): " + hash);
		String output = xorHex(XOR_output, hash);
		
		System.out.println("" + output);
		
		String sender_hash = Sender.getMessageHash();
		String combinedHashes = xorHex(sender_hash, hash);
		String finalOutput = xorHex(combinedHashes, message);
		
		System.out.println("Received from sender: " + output);
		System.out.println("Calculated in receiver: " + finalOutput);
		System.out.println("Message integrity (iff both the above hex strings are equal): " + (output.equals(finalOutput)));
	}
}
