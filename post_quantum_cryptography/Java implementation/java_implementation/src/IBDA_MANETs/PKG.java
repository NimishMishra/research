package IBDA_MANETs;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import Boneh_Lynn_Shacham_scheme.Setup;
import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.jpbc.PairingParametersGenerator;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.a.TypeACurveGenerator;
import it.unisa.dia.gas.plaf.jpbc.pairing.e.TypeECurveGenerator;
import it.unisa.dia.gas.plaf.jpbc.pairing.f.TypeFCurveGenerator;

// PKG is the identity based private key generator 
public class PKG {
	
	// global parameters maintained by PKG
	private static Pairing pairing; // the actual pairing
	private static Field G1;  		 // group G1 of the pairing
	private static Field G2;		 // group G2 of the pairing
	private static Field GT;		 // group Gt of the pairing
	private static Field Zp;		 // multiplicative group of integers module order of G1 (or G2. Both orders are same) 
	private static Element generator_G1; // generator of the cyclic group G1
	
	private static Element masterKey; // an element of Zp
	private static Element masterPublicKey; // generated using master key
	
	private static void generatePairing() {
		// e: G1 x G2 --> GT (Symmetric pairing)
		int rBits = 160;
		int qBits = 512;
		PairingParametersGenerator pairingGenerator = new TypeACurveGenerator(rBits, qBits);
		PairingParameters pairingParameters = pairingGenerator.generate();
		System.out.println("Pairing Parameters: (q + 1 = r * h => such that r is a prime factor of q + 1 where q is a arbitrary large prime)\n" + pairingParameters);
		pairing = PairingFactory.getPairing(pairingParameters);
		System.out.println("Pairing: " + pairing);
		// extract information from the pairing
		G1 = pairing.getG1();
		G2 = pairing.getG2();
		GT = pairing.getGT();
		Zp = pairing.getZr();
		System.out.println("Pairing generated: G1 x G2 --> GT");
		System.out.println("G1: " + G1);
		System.out.println("G2: " + G2);
		System.out.println("GT: "+ GT);
		System.out.println("Zp: " + Zp);
	}
	
	private static void parameterTests() {
		// Tests done to verify how the parameters have been selected
		System.out.println("G1 == G2 ? --> " + G1.equals(G2));
		System.out.println("Order of G1 == order of G2 ? --> " + G1.getOrder().equals(G2.getOrder()));
		BigInteger order = G1.getOrder();
		System.out.println("G1 order prime ? --> " + order.isProbablePrime(1));
		System.out.println("\n(If all are true, parameters have been correctly initialised)");
	}
	private static void generateMasterKeys() {
		generator_G1 = G1.newRandomElement().getImmutable();
		System.out.println("Primitive root of G1 (P): " + generator_G1);
		masterKey = Zp.newRandomElement(); // let this be called s 
		System.out.println("Master key s: " + masterKey);
		masterPublicKey = generator_G1.mulZn(masterKey);
		System.out.println("Master public s.P: " + masterPublicKey);
	}
	
	static Element getHashByH1(String message) {
		// operations related to description of hash function H1
		// H1: {0, 1}* --> G1
		String hashFunction = "SHA-512";
		try {
			MessageDigest digest = MessageDigest.getInstance(hashFunction);
			byte[] inputDigest = digest.digest(message.getBytes());
			BigInteger inputDigestBigInteger = new BigInteger(1, inputDigest);
			String hashText = inputDigestBigInteger.toString(16);
			while(hashText.length() < 32)
				hashText = "0" + hashText;
			Element mappedElement = G1.newElement().setFromHash(hashText.getBytes(), 0, hashText.length());
			return mappedElement;
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}	
	}
	
	private static void pairingTests() {
		Element P = getG1Generator();
		Element Qid = getHashByH1("Nimish");
		Element left = pairing.pairing(P.powZn(masterKey), Qid);
		Element right = pairing.pairing(P, Qid.powZn(masterKey)); // inverted
		System.out.println(left.isEqual(right));
		System.out.println(P);
		System.out.println(Qid);
	}
	
	static String getHashByH2(Element element) {
		// operations related to description of hash function H1
		// H2: {0, 1}p --> GT
		String elementValueInString = element.toString();
		try {
			String hashFunction = "SHA-512";
			MessageDigest digest = MessageDigest.getInstance(hashFunction);
			byte[] inputDigest = digest.digest(elementValueInString.getBytes());
			BigInteger inputDigestBigInteger = new BigInteger(1, inputDigest);
			String hashText = inputDigestBigInteger.toString(16);
			return hashText;			
		}
		catch(NoSuchAlgorithmException e) {
			e.printStackTrace();
			return null;
		}
	}
	
	static void initialisePKG() {
		if(pairing != null) {
			System.out.println("\nPairing already generated. Overwriting it. Make sure to reflect necessary changes by calling other dependents from other classes as well");
		}
		System.out.println("---------------------------Initializing PKG parameters------------------");
		generatePairing();
		System.out.println("\n-------------------------Testing PKG parameters-----------------------");
		parameterTests();
		System.out.println("\n-------------------------Generating master keys-----------------------");
		generateMasterKeys();
		System.out.println("\n------------------------Testing pairing-------------------------------");
		pairingTests();
		System.out.println("\n-------------------------H1 hash function set up ---------------------");
		System.out.println("Hello world --> " + getHashByH1("Hello world"));
		System.out.println("\n-------------------------H2 hash function set up ---------------------");
		Element element = GT.newRandomElement();
		System.out.println("A random element in GT: " + element);
		String elementHashed = getHashByH2(element);
		System.out.println("Hashed into {0, 1}p --> " + elementHashed);
		System.out.println("Length of hash < order of G1/G2 ? --> " + (G1.getOrder().compareTo(BigInteger.valueOf(elementHashed.length()))));
		System.out.println("(Positive value means hash length < order p)");
		
	}
	
	static Element computePrivateKey(Element publicKey) {
		// compute identity based private key
		/*
		 * add code later to validate credentials of the person
		 * claiming to be a certain identity owner 
		 */
		// calculate the private key:
		// D = s.Q where Q = H1(identity) and s is masterKey
		Element D = publicKey.powZn(masterKey);
		return D;
	}
	
	static Element getG1Generator() {return generator_G1;}
	static Element getMasterPublicKey() {return masterPublicKey;}
	static Pairing getPairing() {return pairing;}
	static Element getRandomInZp() {return Zp.newRandomElement();}
}
