import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

//Dijkstra's shortest path algorithm

public class InterstellarTravel {
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("interstellartravel.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			//keep locations in a map to get vertices by name for edge building
			String locations = f.nextLine();
			Scanner temp = new Scanner(locations);
			HashMap<String,Node> map = new HashMap<String,Node>();
			
			//vertex building
			while(temp.hasNext()) {
				String name = temp.next();
				map.put(name, new Node(name));
			}
			temp.close();
			
			//edge building
			int edges = f.nextInt(); f.nextLine();
			while(edges-- > 0) {
				String startV = f.next();
				String endV = f.next();
				int distance = f.nextInt();
				map.get(startV).neighbors.add(map.get(endV)); 
				map.get(startV).weights.add(distance);
			}
			
			//start and end locations
			Node start = map.get(f.next());
			Node destination = map.get(f.next());
			
			//scanners being silly
			if(f.hasNextLine())
				f.nextLine();
			System.out.println(dijkstra(start, destination));
		}
		f.close();
	}
	
	/*
	 * Keep a queue of all nodes to visit, beginning with the starting node. 
	 * 		Distance from starting node starting node is 0.
	 * Remove the first node from the queue, popped. Update all of popped's 
	 * 		neighbors distances with their distances from the start node,
	 * 		then append them to the end of the queue.
	 * Mark popped as visited. 
	 * Once the finishing node has been visited, a path has been found, and it 
	 * 		will be the shortest (line 67).
	 * If the nodes queue becomes empty before destination node is visited, then
	 * 		there exists no path from the start to the destination.
	 */
	public static int dijkstra(Node start, Node end) {
		ArrayList<Node> nodes = new ArrayList<Node>();
		start.dist = 0;
		nodes.add(start);
		while(!nodes.isEmpty()) {
			if(end.visited == true)
				break;
			Node popped = nodes.remove(0);
			for(int i = 0; i<popped.neighbors.size(); i++) {
				Node current = popped.neighbors.get(i);
				current.dist = Math.min(current.dist, popped.weights.get(i) + popped.dist);
				nodes.add(current);
			}
			popped.visited = true;
		}
		
		return end.dist;
	}
	
}
class Node {
	public ArrayList<Node> neighbors;	//list of all vertices this node is connected to
	public ArrayList<Integer> weights;  //parallel list of edge weights for this node to neighbors
	public boolean visited;				//boolean for dijkstra algorithm
	public String name;					//name of this location, completely unnecessary
	public int dist = Integer.MAX_VALUE;//distance to this node from the start node
	
	//Multiple constructors for convenience not necessity
	public Node() {
		neighbors = new ArrayList<Node>();
		weights = new ArrayList<Integer>();
	}
	public Node(String name, ArrayList<Node> neighbors, ArrayList<Integer> weights) {
		this.neighbors = neighbors;
		this.weights = weights;
		this. name = name;
	}
	
	public Node(String name) {
		neighbors = new ArrayList<Node>();
		weights = new ArrayList<Integer>();
		this.name = name;
	}
}
