import java.util.Timer;
import java.util.TimerTask;


public class HouseKeeping {
	//
	Timer timer;
	//
	VocalServer myServer;

	//
	public HouseKeeping(long milliseconds, VocalServer server) {
		myServer = server;
		timer = new Timer();
		timer.schedule(new RemindTask(), 0, milliseconds);
		
	}

	//
	class RemindTask extends TimerTask {
		//
		public void run() {
	
			myServer.handleHouseKeeping();
		}
	}

	//
	public static void main(String args[]) {
		System.out.println("About to schedule task.");
		new HouseKeeping(5*1000, null);
		System.out.println("Task scheduled.");
	}
}
