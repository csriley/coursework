package problems2014;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

/**
 * Given a series of locations and rotations for a cube, display the 2D projection of this cube.
 */
public class Supercomputer {
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("/Users/sno/Documents/workspace/JOwen/src/problems2014/uneven.dat"));
		while(f.hasNextLine()) {
			int [][][] cube = new int[4][4][4]; 
			Scanner chop = new Scanner(f.nextLine());
			while(chop.hasNext()) {
				String command = chop.next();
				if(command.matches("\\d+")) { //returns true if the string is a number
					int index = Integer.parseInt(command);
					cube[index/(cube.length*cube.length)][index%(cube.length*cube.length)/cube.length][index%cube.length]++;
				} else {
					char rotation = command.charAt(0);
					switch(rotation) {
						case 'A': cube = aRotation(cube);break;
						case 'B': cube = bRotation(cube);break;
						case 'C': cube = cRotation(cube);break;
					}
				}
			}
			
			int[][] projection = project(cube);
			print(projection);
		}
	}
	
	/*
	 * Takes the given cube and projects it into 2D space along the FRONT to BACK axis. 
	 */
	private static int[][]  project(int [][][] cube) {
		int [][] projection = new int[cube.length][cube.length];
		for(int i = 0; i < cube.length; i++) {
			for(int j = 0; j < cube.length; j++) {
				for(int k = 0; k < cube.length; k++) {
					projection[i][j] += cube[i][j][k];
				}
			}
		}
		
		return projection;
	}
	
	/*
	 * Displays the given 2D array
	 */
	private static void print(int [][] array) {
		for(int[] i: array) {
			for(int j: i) {
				System.out.print(j + " ");
			}
			System.out.println();
		}
		System.out.println();
	}
	
	//z
	/*
	 * Returns the given 3D array rotated about the FRONT to BACK axis.
	 */
	private static int[][][] aRotation(int[][][] cube) {
		int [][][] tempcube = new int[cube.length][cube.length][cube.length];
		int i = 0;
		for(int x = 0; x < cube.length; x++) {
			int j = 0;
			for(int y = cube.length-1; y >=0; y--) {
				int k = 0;
				for(int z = cube.length-1; z >=0; z--) {
					tempcube[i][j][k] = cube[x][y][z];
					k++;
				}
				j++;
			}
			i++;
		}
		
		return tempcube;
	}
	
	//y
	/*
	 * Returns the given 3D array rotated about the TOP to BOTTOM axis.
	 */
	private static int[][][] bRotation(int[][][] cube) {
		int [][][] tempcube = new int[cube.length][cube.length][cube.length];
		int i = 0;
		for(int x = cube.length-1; x >=0; x--) {
			int j = 0;
			for(int y = 0; y < cube.length; y++) {
				int k = 0;
				for(int z = cube.length-1; z >=0; z--) {
					tempcube[i][j][k] = cube[x][y][z];
					k++;
				}
				j++;
			}
			i++;
		}
		
		return tempcube;
	}
	
	//x
	/*
	 * Returns the given 3D array rotated about the LEFT to RIGHT axis.
	 */
	private static int[][][] cRotation(int[][][] cube) {
		int [][][] tempcube = new int[cube.length][cube.length][cube.length];
		int i = 0;
		for(int x = cube.length-1; x >=0; x--) {
			int j = 0;
			for(int y = cube.length-1; y >=0; y--) {
				int k = 0;
				for(int z = 0; z < cube.length; z++) {
					tempcube[i][j][k] = cube[x][y][z];
					k++;
				}
				j++;
			}
			i++;
		}
		
		return tempcube;
	}
}