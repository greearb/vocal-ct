import java.*;

public class HeartbeatData {
	//
	String type;
	//
	String port;
	//
	String value;
	//
	private static String byteToShort(byte msb, byte lsb, boolean swap) {

		int imsb = new Byte(msb).intValue();
		imsb = (imsb << 8) & (0x0000FF00) ;

		int ilsb = new Byte(lsb).intValue();
		ilsb = (ilsb) & (0x000000FF) ;

		int ret = 0;
		ret = imsb | ilsb;

		return ret + "";
	}

	//
	public HeartbeatData( byte[] data ) {

		type = byteToShort(data[0], data[1], true);
		port = byteToShort(data[4], data[5], true);
		value = byteToShort(data[6], data[7], true);
	}
	//
	public String getType() {
		return type;
	}
	//
	public String getPort() {
		return port;
	}
	//
	public String getValue() {
		return value;
	}

	//
	public String toString() {
		return "Type: " + type + " Port: " + port  + " Value: " + value;

	}
}
