import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

/*
 * Shortest path from multiple endpoints.
 */
public class Monsters {
	public static final int BOARD_HEIGHT = 7;
	public static final int BOARD_WIDTH = 10;

	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("monsters.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			
			//input
			MapPoint [][] board = new MapPoint[BOARD_HEIGHT][BOARD_WIDTH];
			ArrayList<MapPoint> monsters = new ArrayList<MapPoint>();
			ArrayList<MapPoint> exits = new ArrayList<MapPoint>();
			MapPoint start = null;
			for(int i = 0; i<BOARD_HEIGHT; i++) {
				String in = f.nextLine();
				for(int j = 0; j<BOARD_WIDTH; j++) {
					if(j<=in.length()) {
						in += ".";
					}
					board[i][j] = new MapPoint(i, j, in.charAt(j));
					if(board[i][j].let == 'J') {
						start = board[i][j];
					}
					if(board[i][j].let == 'M') {
						monsters.add(board[i][j]);
					}
					if(board[i][j].let == 'E') {
						exits.add(board[i][j]);
					}
				}
			}
			
			setAdjacencies(board);
			setMonsterReach(board, monsters);
			
			//makes the start location travelable
			start.let = '.';
			//ensures each direction is traveled and thus the shortest is chosen
			for(int i = 0; i<8; i++) {
				clearBoard(board);
				//begin at exits and look for the start
				for(MapPoint point: exits) {
					point.closestExit = 0;
					if(point.let == 'E')
						point.let = '.';
					compute(point,start);
				}
			}
			System.out.println("The closest exit is " + start.closestExit + " moves away.");
		}
		f.close();
	}
	
	/*
	 * Sets each MapPoint to its neighbors. Since we can travel diagonally, the 8 different directions are set 
	 */
	public static void setAdjacencies(MapPoint [][] board) {
		for(int i = 0; i<BOARD_HEIGHT; i++) {
			for(int j = 0; j<BOARD_WIDTH; j++) {
				if(i-1 >= 0)
					board[i][j].adj[0] = board[i-1][j];
				if(i-1 >= 0 && j+1 < BOARD_WIDTH)
					board[i][j].adj[1] = board[i-1][j+1];
				if(j+1<BOARD_WIDTH)
					board[i][j].adj[2] = board[i][j+1];
				if(i+1 < BOARD_HEIGHT && j+1 < BOARD_WIDTH)
					board[i][j].adj[3] = board[i+1][j+1];
				if(i+1 < BOARD_HEIGHT)
					board[i][j].adj[4] = board[i+1][j];
				if(i+1 < BOARD_HEIGHT && j-1 >=0)
					board[i][j].adj[5] = board[i+1][j-1];
				if(j-1 >= 0)
					board[i][j].adj[6] = board[i][j-1];
				if(i-1 >= 0 && j-1>=0)
					board[i][j].adj[7] = board[i-1][j-1];
			}
		}
	}
	
	/*
	 * The monsters have a reach of one, meaning we cannot get within one step of them.
	 */
	public static void setMonsterReach(MapPoint [][] board, ArrayList<MapPoint> monsters) {
		for(MapPoint point: monsters) {
			for(MapPoint adjs: point.adj) {
				if(adjs != null)
					adjs.let = 'M';
			}
		}
	}
	
	/*
	 * Makes the board seem unvisited while keeping all the distances the same.
	 */
	public static void clearBoard(MapPoint [][] board ) {
		for(int i = 0;i < BOARD_HEIGHT; i ++) {
			for(int j = 0; j<BOARD_WIDTH; j++) {
				board[i][j].visited =false;
			}
		}
	}
	
	/*
	 * Looks at each MapPoint and see if it is travelable, and how far away from an exit it is.
	 */
	public static void compute(MapPoint point, MapPoint start) {
		
		if(point == null || point.visited || point.let != '.') {
			return;
		}
		
		int minDist = BOARD_HEIGHT * BOARD_WIDTH;
		for(MapPoint adjPoint: point.adj) {
			if(adjPoint!=null)
				minDist = Math.min(minDist, adjPoint.closestExit);
		}
		point.closestExit = Math.min(minDist + 1, point.closestExit);
		point.visited = true;
		for(MapPoint adjPoint: point.adj) {
			compute(adjPoint, start);
		}
	}
}

class MapPoint {
	public int locRow;
	public int locCol;
	public char let;
	public int closestExit = Monsters.BOARD_HEIGHT * Monsters.BOARD_WIDTH;
	public boolean visited = false;
	public MapPoint [] adj = new MapPoint[8];
	public MapPoint(int i, int j, char c) {
		locRow = i;
		locCol = j;
		let = c;
	}
	
	public String toString() {
		return let + "-" + closestExit;
	}
}
