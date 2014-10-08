import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Controller {
  private static Process[] processes;
  private static PrintWriter[] streams;
  
  private static Lock lock = new ReentrantLock();
  private static Condition allMastersNotified = lock.newCondition();
  private static int [] masterIds = new int[0];
  private static int notified = 0;
  
  static int numNodes = 0;
  public static void main(String [] args) {
	processes = new Process[64511];
	streams = new PrintWriter[64511];
    Scanner scan = new Scanner(System.in);
    
    Runtime.getRuntime().addShutdownHook(new Thread()
    {
        public void run()
        {
            for(int i = 0; i < numNodes; i++) {
        		processes[i].destroy();
        	}
        }
    });
    
    while (scan.hasNextLine()) {	
      String [] inputLine = scan.nextLine().split(" ");
      int nodeIndex, node1Index, node2Index;
      switch (inputLine[0]) {
        case "CreateNodes":
            /* 
            This commands starts this number of nodes in the
            system. These nodes should be accessible from the
            controller program by their index in [0,n - 1].
             */
            numNodes = Integer.parseInt(inputLine[1]);
            
            for(int i = 0; i < numNodes; i++) {
            	try {
					processes[i] = JavaProcess.exec(Node.class, ((Integer)i).toString()).start();
					streams[i] = new PrintWriter(processes[i].getOutputStream());
				} catch (IOException ioe) {
					System.err.println("EXCEPTION ON CREATE NODES "+i+": ");
					ioe.printStackTrace();
				}
            }
            break;
        case "CreateConnection":
            /*
            This command creates a bidirectional communica-
            tion channel through the nodes with index i and j in
            the system. We will call this function many times to
            establish the network topology before we issue any
            other commands.
             */
            node1Index = Integer.parseInt(inputLine[1]);
            node2Index = Integer.parseInt(inputLine[2]); 
            
            streams[node1Index].println("connect " + node2Index);
            streams[node1Index].flush();
                       
            streams[node2Index].println("connect " + node1Index);
            streams[node2Index].flush();
                        
            break;
        case "SendMessage":
            /*
            This command sends a message from node i to node
            j. This operation should increment the operation
            counter of each node by one for the send and receive
            events respectively. Note that we will only call this
            if there is a connection between the nodes directly.
             */
            node1Index = Integer.parseInt(inputLine[1]);
            node2Index = Integer.parseInt(inputLine[2]);
            
            streams[node1Index].println("send " + node2Index);
            streams[node1Index].flush();
            
            break;
        case "LocalIncrement":
            /*
            This command instructs the node i to increment its
            operation counter.
             */
            nodeIndex = Integer.parseInt(inputLine[1]);
            streams[nodeIndex].println("increment");
            streams[nodeIndex].flush();
            break;
        case "MakeMasters":
            /*
            This command is used to instruct all nodes in the sys-
            tem that are going to be masters in the coming snap-
            shot. It will always be called before we call Start-
            Snapshot. The reason this is separate from Start-
            Snapshot is to ensure that you don’t mistakenly tell
            the masters sequentially to start the protocol because
            if it’s too fast, one master may do all the work. Note
            that you will need to inform each master node of the
            number of other masters so that each knows when it
            has received the snapshot information from all other
            masters (and therefore that the protocol is finished).
             */
        	
        	for(int i = 0; i < numNodes; i++) {
            	streams[i].println("purge");
            	streams[i].flush();
            }
        	
            int numMasters = inputLine.length - 1;
            masterIds = new int[numMasters];
            for (int i = 1; i < inputLine.length; i++) {
              masterIds[i - 1] = Integer.parseInt(inputLine[i]);
              streams[masterIds[i-1]].println("masters " + numMasters);
              streams[masterIds[i-1]].flush();
            }
            
            break;
        case "StartSnapshot":
            /*
            This command initializes a snapshot with the mas-
            ters specified in the preceding MakeMasters com-
            mand. This command should block until all masters
            have received all snapshot information. This can be
            done by having the controller thread wait until it
            hears a confirmation message from each master.
             */
        	for(int i = 0; i < masterIds.length; i++) {
        		Thread thread = new Thread(((Integer)masterIds[i]).toString()) {
        			public void run() {
	    		      lock.lock();
	    		      int masterToNotify = Integer.parseInt(getName());
					  notified += 1;
					  if(notified < masterIds.length){
						try {
							allMastersNotified.await();
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					  }
					  
					  if(notified == masterIds.length) {
						  allMastersNotified.signal();
					  }
					  streams[masterToNotify].println("snapshot");
					  streams[masterToNotify].flush();
					  lock.unlock();
        			}
        		};
        		thread.start();
        	}	
            break;
        case "PrintSnapshot":
            /*
            This command instructs a node to print the full state
            of the most recent snapshot it partook in (in the for-
            mat specified in the next section). Note that this
            will only be called on a master node right after Start-
            Snapshot unblocks.
             */
        	
			 try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
            nodeIndex = Integer.parseInt(inputLine[1]);
            streams[nodeIndex].println("print");
            streams[nodeIndex].flush();
            break;
        case "Shutdown":
        	for(int i = 0; i < numNodes; i++) {
        		processes[i].destroy();
        	}
        	break;
      }
    }
  }
}
