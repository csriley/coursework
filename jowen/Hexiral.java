import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;


public class Hexiral {
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("hexiral.dat"));
		while(f.hasNext()) {
			HexBoard hex = new HexBoard();
			String word = f.next();
			hex.fillWithWord(word);
			hex.print();
			System.out.println();
		}
	}
}

class HexBoard {
	public char [][] board;
	public int currentx;			//the current x location letters are being set
	public int currenty;			//the current y location letters are being set
	public int nextx;				//the next location of x
	public int nexty;				//the next location of y
	private int counter = 0; 		//number of letters put on this side
	private int maxCounter = 5;		//maximum number of letters in this side
	private int sideCounter = 0;	//the side of the hexagon we are currently putting letters on
	private int hexCounter = 0;		//how many complete revolutions around the perimeter 
	private int xdir = 1;			//direction x is increasing on this side
	private int ydir = 0;			//direction y is increasing on this side
	
	public HexBoard() {
		board = new char[11][16];
		for(int x = 0; x<board.length; x++)
			Arrays.fill(board[x], ' ');
		
		//start on top left corner of hexagon
		currentx = 5;
		currenty = 0;
		nextx = 6;
		nexty = 0;
	}
	
	/*
	 * Sets the given character at the current location of x and y.
	 * Returns true when the last character of the hexagon is set.
	 */
	private boolean set(char c) {
		board[currenty][currentx] = c;
		currentx = nextx;
		currenty = nexty;
		return calcNextPosition();
	}
	
	/*
	 * Calculates the next position dependant on the side we are currently on.
	 * Returns true when the last character of the hexagon is set.
	 */
	private boolean calcNextPosition() {
		
		counter++;
		
		//hard coded values for when we have to fill the last row and when the board is full.
		if(board[5][10]!=board[0][0]) {
			return true;
		}
		
		//last line of the board, fill in the middle
		if(board[6][5] != board[0][0]) {
			fillLastLine();
			return false;
		}
		
		//side full; change sides
		if(counter == maxCounter) {
			counter = 0;
			sideCounter++;
			switch(sideCounter) {
			//
			//     6661
			//    5    1		 
			//   5      1
			//  4  SIDES 2
			//   4      2
			//    4    2
			//     3333    
			//
			case 1: {
				xdir = 1; 
				ydir = 1;
				} break;
			case 2: {
				xdir = -1; 
				ydir = 1;
				} break;
			case 3: {
				xdir = -1; 
				ydir = 0;
				counter-=hexCounter;
				} break;
			case 4: {
				xdir = -1; 
				ydir = -1;
				} break;
			case 5: {
				xdir = 1; 
				ydir = -1; 
				counter++;
				} break;
			case 6: {
				xdir = 1; 
				ydir = 0;
				counter-=hexCounter+2;
				} break;
			}
		}
		
		//full circle
		if(sideCounter == 6) {
			sideCounter = 0;
			maxCounter--;
			hexCounter++;
		}
		
		nextx = nextx + xdir;
		nexty = nexty + ydir;
		
		return false;
	}
	
	/*
	 * Prints this hexagon on the console.
	 */
	public void print(){
		for(int y = 0; y<board.length; y++) {
			for(int x = 0; x<board[0].length; x++) {
				System.out.print(board[y][x]);
			}
			System.out.println();
		}
	}
	
	/*
	 * Calls set on the characters from this word sequentially until the hexagon is filled.
	 */
	public void fillWithWord(String word) {
		boolean done = false;
		while(!done) {
			for(int x = 0; x<word.length(); x++) {
				if(done)
					continue;
				done = this.set(word.charAt(x));
			}
		}
	}
	
	/*
	 * Filling the middle of the hexagon on the last side iteration is just a horizontal
	 * line thorough the center.
	 */
	private void fillLastLine() {
		nextx = nextx+1;
	}
}
