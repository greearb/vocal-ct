import java.io.*;
import java.net.*;
import java.util.*;

class HeartbeatReceiver {

	//
	InetAddress multicastAddress = null;
	//
	int multicastPort = 0;
	//
        MulticastSocket socket = null;
	//
	public HeartbeatReceiver (String Address, int Port) throws IOException {
		multicastPort = Port;
	        multicastAddress = InetAddress.getByName(Address);

        	socket = new MulticastSocket(multicastPort);
   		socket.joinGroup(multicastAddress);
	}

	//
	public String toString() {
		return "HeartbeatReceiver: " + multicastAddress.getHostName() + "#" + multicastAddress.getHostAddress() + ":" + multicastPort ;
	}

	//
	public void listeningForHeartbeat(VocalServer server) throws IOException {
		System.out.println( "Listening for heartbeats ... " );
		for ( ; ; ) {
		        DatagramPacket packet;
    
       		     	byte[] buf = new byte[256];
       		     	packet = new DatagramPacket(buf, buf.length);
       		     	socket.receive(packet);

       		     	String receivedHeartbeat = new String(packet.getData());
	
			server.handleHeartbeat( packet );
		}
	}

	//
	protected void  finalize() {
		System.out.println( "Cardiac Arrest..." );

		if (socket == null) return;
		try {
		        socket.leaveGroup(multicastAddress);
       			socket.close();
		}
		catch (IOException e) {
			System.out.println( "Exception Thrown on Socket close\n" );
			e.printStackTrace();
		}
	}
}
