package java_implementation;

import java.io.FileInputStream;
import java.security.*;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.spec.ECGenParameterSpec;

import javax.crypto.*;
import org.bouncycastle.jce.provider.BouncyCastleProvider;


public class key_generation {

	public static KeyPair generateKeyPair(String text) throws GeneralSecurityException
	{
			KeyPairGenerator keyPair = KeyPairGenerator.getInstance("EC", "BC");
			keyPair.initialize(new ECGenParameterSpec(text));
			return keyPair.generateKeyPair();
	}
	
	public static void function() throws Exception {
		Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
		
		// P-384 curve
		KeyPair generatedKeys = generateKeyPair("P-384");
		System.out.println("------------------------ P-384 curve --------------------------");
		System.out.println("Private key: \n" + generatedKeys.getPrivate());
		System.out.println("Public key: \n" + generatedKeys.getPublic());
		System.out.println("-------------------------------------------------------------");
		
		// P-256 curve
		
		System.out.println("------------------------ P-256 curve --------------------------");
		generatedKeys = generateKeyPair("P-256");
		System.out.println("Private key: \n" + generatedKeys.getPrivate());
		System.out.println("Public key: \n" + generatedKeys.getPublic());
		System.out.println("-------------------------------------------------------------");
		
		// P-224
		
		System.out.println("------------------------ P-224 curve --------------------------");
		generatedKeys = generateKeyPair("P-224");
		System.out.println("Private key: \n" + generatedKeys.getPrivate());
		System.out.println("Public key: \n" + generatedKeys.getPublic());
		System.out.println("-------------------------------------------------------------");
		
		// P-521
		System.out.println("------------------------ P-521 curve --------------------------");
		generatedKeys = generateKeyPair("P-521");
		System.out.println("Private key: \n" + generatedKeys.getPrivate());
		System.out.println("Public key: \n" + generatedKeys.getPublic());
		System.out.println("-------------------------------------------------------------");
	}
}
