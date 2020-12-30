package Boneh_Lynn_Shacham_scheme;

import it.unisa.dia.gas.jpbc.Element;

public class KeyGeneration {
	private static Element privateKey;
	private static Element publicKey;
	
	static void initialiseSetup() {
		Setup.initialisePairing();
		Setup.findGenerator();
		System.out.println("\n--------------------Key Generation-------------");
	}
	static void generatePrivateKey() {
		// in BLS scheme, private key is a random element
		// in Zr group
		privateKey = Setup.getPairing().getZr().newRandomElement();
		System.out.println("Private key generated: " + privateKey);
	}
	
	static void generatePublicKey() {
		publicKey = Setup.getGenerator().powZn(privateKey);
		System.out.println("Public key generated: " + publicKey);
	}
	
	static Element getPublicKey() {return publicKey;}
	static Element getPrivateKey() {return privateKey;}
	
	static void initialiseKeyGeneration() {
		initialiseSetup();
		generatePrivateKey();
		generatePublicKey();
	}
}
