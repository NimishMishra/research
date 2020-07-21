package Boneh_Lynn_Shacham_scheme;

import java.math.BigInteger;

import it.unisa.dia.gas.jpbc.*;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.a.TypeACurveGenerator;

public class Setup {
	private static PairingParametersGenerator pg;
	private static PairingParameters pairingParams;
	private static Pairing pairing;
	
	private static Element g;  
	
	
	private static final int rBits = 160;
	private final static int qBits = 512;
	
	
	// Type A curve generation details: https://crypto.stanford.edu/pbc/manual/ch08s03.html
	static void initialisePairing() {
		// Generate a Type A elliptic curve
		pg = new TypeACurveGenerator(rBits, qBits);
		
		// Generate pairing parameters based on that curve
		pairingParams = pg.generate();
		System.out.println("------------ Setup -------------------------");
		System.out.println("Pairing parameters: " + pairingParams + "\n");
		
		// Generate pairing based on those pairing parameters
		pairing = PairingFactory.getPairing(pairingParams);
		System.out.println("Pairing: " + pairing);
		
	}

	static void findGenerator() {
		// Element g is the generator of the symmetric pairing being used here
		// Since G1 = G2, it doesn't really matter 
		g = pairing.getG1().newRandomElement().getImmutable();
		System.out.println("\nGenerator: " + g);
	}
	
	static Pairing getPairing() {return pairing;}
	static Element getGenerator() {return g;}
}
