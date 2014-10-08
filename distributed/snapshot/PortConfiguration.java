
public class PortConfiguration {
	private static int [] ports = new int[64511];
	static {
		bindPorts();
	}
	
	public static void bindPorts() {
		for(int i = 0; i < ports.length; i++) {
			ports[i] = i + 1025;
		}
	}
	
	public static int getPort(int index) {
		return ports[index];
	}
}
