import java.io.*;
import java.net.*;
import java.util.*;


public class VocalMon {


	public static void main(String[] args) throws IOException {

	usage(args);

	HeartbeatReceiver mcReceiver = new HeartbeatReceiver( args[0], Integer.parseInt(args[1]) );
	VocalServer	  vocal = new VocalServer();

	HouseKeeping lookForServerStats = new HouseKeeping ( HeartbeatSender.heartbeatInterval, vocal);

	mcReceiver.listeningForHeartbeat(vocal);

	mcReceiver = null;

    }

    private static void usage(String[] args) {

		if (args.length != 4) {
			System.out.println ("\n\n\nUsage: VocalMon " +   " MulticastAddress MulticastPort HeartbeatInterval(ms) MaxMissedHeartbeat\n\n" );	
			System.exit( -1 );
		}
		else {
			System.out.println ("\n\n\nMulticastAddress = " + args[0] + ":" + args[1] + ", HeartbeatInterval(ms) = " + args[2] + ", MaxMissedHeartbeat = " + args[3] + ".\n\n" );	

			HeartbeatSender.heartbeatInterval = Long.parseLong( args[2] );
			HeartbeatSender.maxMissedHeartbeat = Integer.parseInt( args[3] );

		}
	}
}
