package IBDA_MANETs;

public class Test {
	
	public static void main(String [] args) {
		PKG.initialisePKG();
		Sender.initialiseSender("Nimish");
		Receiver.initialiseReceiver("Gaurang");
		Sender.deniableAuthentication("abcd");
		Receiver.receiveFromSender();
	}
}
