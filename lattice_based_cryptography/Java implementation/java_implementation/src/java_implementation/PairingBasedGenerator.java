package java_implementation;

import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;

public class PairingBasedGenerator {
	Pairing pairing = PairingFactory.getPairing("params.properties");
	
	Field G1 = pairing.getG1();
	
}
