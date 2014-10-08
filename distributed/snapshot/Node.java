import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;
import java.util.TreeSet;

public class Node {
	private static int nodeId;
	private static int localColor;
	private static int parentId;
	private static int snapshotCounter;
	private static int localCounter;
	
	private static HashSet<Integer> children = new HashSet<Integer>();
	private static HashSet<Integer> warningReceived = new HashSet<Integer>();
	private static HashSet<Integer> doneReceived = new HashSet<Integer>();
	private static HashSet<Integer> otherMasters = new HashSet<Integer>();
	private static TreeSet<String> data = new TreeSet<String>();
	private static String state;
	private static int round = 0;
		
	public static HashMap<Integer, Socket> outgoing = new HashMap<Integer, Socket>();
	public static HashMap<Integer, Socket> incoming = new HashMap<Integer, Socket>();
	public static int numMasters = -1; //if you aren't a master then you don't know
	
	
	public static void main(String [] args) throws IOException {
		nodeId = Integer.parseInt(args[0]);
		localCounter = 0;
		parentId = -1;
		snapshotCounter = 0;
		localColor = -1; //white
		
		//runs a thread to listen for communications from the controller
		Thread thread = new Thread() {
		     public void run() {
		    	 Scanner in = new Scanner(System.in);	
		    	 //System.out.println(nodeId + " LISTENING ");
		    	 while(in.hasNextLine()) {	 
	 				String [] inputLine = in.nextLine().split(" ");
	 				String command = inputLine[0];
	 				if(command.equals("connect")) {
	 					int index = Integer.parseInt(inputLine[1]);
	 					connectClient(index);
	 					//System.out.println(nodeId + " OUTGOING SOCKET BOUND");
	 				}
	 				
	 				if(command.equals("increment")) {	
	 					increment();
	 					//System.out.println("INCREMENT " + nodeId + " TO " + localCounter);  
	 				}
	 				
	 				if(command.equals("send")) {
	 					int index = Integer.parseInt(inputLine[1]);
	 					send(index, "message");
	 				}
	 				
	 				if(command.equals("masters")) {
	 					int numMasters = Integer.parseInt(inputLine[1]);
	 					masterNotification(numMasters);
	 				}
	 				
	 				if(command.equals("purge")){
	 					reset();
	 				}
	 				
	 				if(command.equals("snapshot")) {
	 					beginSnapshot();
	 				}
	 				
	 				if(command.equals("print")) {
	 					print();
	 				}
		 		}		 		
		     }
		};	
		thread.start();

		ServerSocket server = new ServerSocket(PortConfiguration.getPort(nodeId));
		while(true) {
			int index = incoming.size()-1;
			incoming.put(index, server.accept());
			Thread newClientThread = new Thread(((Integer) index).toString()) {
				public void run() {
					//System.out.println(nodeId + " INCOMING SOCKET BOUND");
					int index = Integer.parseInt(getName());
					try {
						BufferedReader reader = new BufferedReader(new InputStreamReader(incoming.get(index).getInputStream()));
						while(true) {	
							receive(reader);
						}
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			};
			newClientThread.start();
		}
	}
	
	public static void connectClient(int index) {
		try {
			outgoing.put(index, new Socket(InetAddress.getLoopbackAddress(), PortConfiguration.getPort(index)));
		} catch (IOException e) {
			System.err.println("EXCEPTION CONNECTING SOCKET " + nodeId +" TO " + index +": ");
			e.printStackTrace();
		}
	}
	
	public static void increment() {
		++localCounter;
	}
	
	public static void send(int index, String message) {
		try {
			//System.out.println("SEND FROM " + nodeId + " TO " + index + ": " + message);
			PrintWriter writer = new PrintWriter(outgoing.get(index).getOutputStream());
			writer.println(message);
			writer.flush();
			if(message.equals("message")) {
				++localCounter; //send counter
			}			
		} catch (IOException e) {
			System.err.println("EXCEPTION IN SEND FROM " + nodeId + " TO "+ index);
			e.printStackTrace();
		} catch (NullPointerException ne) {
			System.err.println(index + " AND " + nodeId + " HAVE NO ESTABILISHED CONNECTION.");
			ne.printStackTrace();
		}
	}
	
	public static void receive(BufferedReader reader) {
		try {
			String line = reader.readLine();
			int sender;
			if(line != null) {
				String[] message = line.split(" ");
				//System.out.println("MESSAGE RECIEVED BY " + nodeId + ": " + line);
				switch(message[0]) {
				case "message" : 
					++localCounter; //receive counter
					break;
				case "warning": 
					int master = Integer.parseInt(message[1]);
					sender = Integer.parseInt(message[2]);
					getWarning(master, sender);
					break;
				case "child" :
					sender = Integer.parseInt(message[1]);
					getChild(sender);
					break;
				case "done" :
					sender = Integer.parseInt(message[1]);
					getDone(sender);
					break;
				case "data" :
					String info = "";
					for(int i = 1; i < message.length; i++) {
						info += message[i] + " ";
					}
					getData(info);
					break;
				case "diss" :
					String info2 = "";
					for(int i = 1; i < message.length; i++) {
						info2 += message[i] + " ";
					}
					getDiss(info2);
					break;
					
				}			
			}		
		} catch (IOException e) {
			System.err.println("EXCEPTION IN RECEIVE BY " + nodeId);
			e.printStackTrace();
		}
	}
	
	public static void masterNotification(int numMasters) {
		//System.out.println("MASTER NOTIFIED " + nodeId);
		parentId = nodeId;
		localColor = nodeId;
		Node.numMasters = numMasters;
	}
	
	public static void beginSnapshot() {
		//System.out.println("BEGINNING SNAPSHOT AT " + nodeId);
		changeColor(nodeId, nodeId); //sending out a wave of warnings matching own color
	}
	
	private static void changeColor(int warningColor, int senderColor) {
		snapshotCounter++;
		localColor = warningColor;
		parentId = senderColor;
		if(parentId != nodeId && parentId != -1) {
			send(parentId, "child " + nodeId);
		}
		
		state = nodeId + ", " + localColor + ", " + parentId + ", " +  snapshotCounter + ", " + localCounter;
		
		//Since this data is never requested, I didn't store it, but this is where you would store it.
		//for all incoming channels
			//record messages
		
		for(int i:outgoing.keySet()) {
			send(i, "warning " + warningColor + " " + nodeId);
		}
	}
	
	private static void getWarning(int warningColor, int senderColor) {
		if(localColor == -1) {
			changeColor(warningColor, senderColor);
		}
		
		if(warningColor != localColor) {
			//borders.add(warningColor); //dont need borders if masters are guaranteed to be connected.
		}
		
		if(warningColor == senderColor && warningColor != nodeId) {
			//System.out.println("NOTIFIED OF OTHER MASTER " + nodeId + " OF " + senderColor);
			otherMasters.add(senderColor);
		}
		
		warningReceived.add(senderColor);
		if(warningReceived.size() == incoming.size() && children.size() == 0) {
			if(parentId != nodeId) {
				send(parentId, "data " + state);
				send(parentId, "done " + nodeId);
			}
		}
	}
	
	private static void getChild(int childId) {
		children.add(childId);
	}
	
	private static void getDone(int childId) {
		doneReceived.add(childId);
		if(doneReceived.size() == children.size() && warningReceived.size() == incoming.size()) {
			if(parentId != nodeId) {
				send(parentId, "data " + state);
				send(parentId, "done " + nodeId);
			} else {
				initiateDissemination();
			}
		}		
	}
	
	private static void getData(String info) {
		state += " ::: " + info;
		storeData(state);
	}
	
	private static void storeData(String info) {
		String [] pieces = info.split(" ::: ");
		for(String piece: pieces) {
			data.add(piece);
		}
	}
	
	private static void getDiss(String info) {
		state += " ::: " + info;
		storeData(state);
		round++;
		if(round < numMasters){
			for(int master: otherMasters) {
				//System.out.println(nodeId + "DISSEMINATE " + master);
				send(master, "diss " + state);
			}
		} 
	}
	
	private static void reset() {
		children = new HashSet<Integer>();
		warningReceived = new HashSet<Integer>();
		doneReceived = new HashSet<Integer>();
		otherMasters = new HashSet<Integer>();
		data = new TreeSet<String>();
		state = "";
		round = 0;
		localColor = -1;
		parentId = -1;
	}
	
	private static void print() {
		for(String piece: data) {
			System.out.println(piece);
		}
	}
	
	//if masters are guaranteed to be connected, then we can identify masters by those whose warningcolor matches their nodeid.
	private static void initiateDissemination() {
		for(int master: otherMasters) {
			send(master, "diss " + state);
		}
	}
}
