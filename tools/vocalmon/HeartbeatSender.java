import java.io.*;
import java.net.*;
import java.util.*;

class HeartbeatSender {
	//
	InetAddress myAddress = null;
	//
	String	    myPort;
	//
	int	    myMissedHeartbeat = 0;
	//
	long	    lastHeartbeat = 0;
	//
	boolean     active = false;
	//
	public static long heartbeatInterval;
	//
	public static int maxMissedHeartbeat;
	//
	public HeartbeatSender(InetAddress senderMulticastAddress, String port, byte[] heartbeatData ) {
		myAddress = senderMulticastAddress;
		myPort = port;
		receivedHeartbeat();
	}
	//
	public void incMissedHeartbeat() {
		myMissedHeartbeat++;
		if ( isDown() ) {
			if (active == true) {
				System.out.println (this + " ** DOWN ** ");
				active = false;
			}
		}
	}
	//
	public void receivedHeartbeat() {
		lastHeartbeat =  System.currentTimeMillis();
		myMissedHeartbeat = 0;
		if (active == false)	{
			System.out.println (this + " ** UP **");
		}
		active = true;
	}

	public boolean isDown( ) {
		if ( ( lastHeartbeat + heartbeatInterval < System.currentTimeMillis() ) && (myMissedHeartbeat >=  maxMissedHeartbeat ) ) {
			return true;
		}
		else {
			return false;
		}
	}
	
	public String toString() {
		return "HeartbeatSender: " + myAddress.getHostName() + "#" + myAddress.getHostAddress() + ":" + myPort +
			", MissedHeartbeat: " + myMissedHeartbeat  + " Last Heartbeat: " + lastHeartbeat;
	}
}
