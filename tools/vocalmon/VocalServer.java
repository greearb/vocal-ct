import java.io.*;
import java.net.*;
import java.util.*;

class VocalServer {

	//
	Map	myServers;

	//
	public VocalServer() {
		myServers = new TreeMap();
	}

	//
	void handleHeartbeat( DatagramPacket rxHeartbeatData ) {

		HeartbeatData  hbData = new HeartbeatData( rxHeartbeatData.getData() );
		String Address = rxHeartbeatData.getAddress().getHostAddress();
		String Port =  hbData.getPort();

		String key = Address  + ":" + Port;

		Map m = Collections.synchronizedMap( myServers );
		synchronized(m) {
			HeartbeatSender hbSender  = (HeartbeatSender) myServers.get( key );

			if (hbSender == null ) {
				hbSender  = new HeartbeatSender( rxHeartbeatData.getAddress(), Port, rxHeartbeatData.getData() );
				myServers .put( key, hbSender );
			}
			else	
				hbSender.receivedHeartbeat();
		}
	}

	public void handleHouseKeeping() {

		Map m = Collections.synchronizedMap( myServers );
		synchronized(m) {
			Iterator i = m.keySet().iterator();
			while (i.hasNext()) {
				try {
					Object key =  i.next();

					HeartbeatSender hbSender  = (HeartbeatSender) m.get( key );
					hbSender.incMissedHeartbeat();
				}
				catch (Exception e) { 
					e.printStackTrace();
 				}

			}
			
		}
	}

	//
	public String toString() {
	        return "Number of Vocal Servers: "  + myServers.size()  + "Vocal Servers: " + myServers; 
	}
}
