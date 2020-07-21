package Boneh_Lynn_Shacham_scheme;

import java.math.BigInteger;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.jpbc.PairingParametersGenerator;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.f.TypeFCurveGenerator;

public class MasterClass {
	
	private static String message = "I am Nimish Mishra";
	
	public static void main (String [] args){
		KeyGeneration generator = new KeyGeneration();
		generator.initialiseKeyGeneration();
		
		MessageSign signature = new MessageSign();
		signature.initialiseMessageSigning(message);
		
		Verification verify = new Verification();
		verify.verifySignature(message);
	}
}
