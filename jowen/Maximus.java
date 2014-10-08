package problems2014;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

/**
 * Given an ordered list of numbers, inserting only (, ), +, and * return the maximum value that this ordered 
 * list can achieve. 
 */
public class Maximus {
	//contains all possible permutations of + and *
	private static Set<ArrayList<Integer>> allSolutions = new HashSet<ArrayList<Integer>>();
	
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("/Users/sno/Documents/workspace/JOwen/src/problems2014/maximus.dat"));
		while(f.hasNextLine()) {
			String input = f.nextLine();
			String[] values = input.split(" ");
			int [] nums = new int[values.length];
			for(int i = 0; i < values.length; i ++) {
				nums[i] = Integer.parseInt(values[i]);
			}
			
			allSolutions = new HashSet<ArrayList<Integer>>();
			
			ArrayList<Integer> prefix = new ArrayList<Integer>();
			for(int i = 0; i < nums.length-1; i++) {
				prefix.add(i);
			}
			for(int i = 0; i < nums.length; i++) {
				generatePlusIndices(prefix, new ArrayList<Integer>(), i);
			}
			
			int max = 0;
			for(ArrayList<Integer> formula: allSolutions) {
				int total = computeTotal(formula, nums);
				//if we have found a total that is larger, update
				if(max < total) {
					max = total;
				}
			}
			System.out.println(max);
		}	
	}
	
	/*
	 * Returns the computed value for this formula, where formula is the indices of
	 * the additions corresponding to this solution for nums.
	 */
	private static int computeTotal(ArrayList<Integer> formula, int [] nums) {
		int [] temp = Arrays.copyOf(nums, nums.length);
		
		//adds together all pieces for the associated additions
		for(int i = 0; i < nums.length; i++) {
			if(formula.contains(i)) {
				temp[i+1] = temp[i] + temp[i+1];
				temp[i] = 1; //replacing with 1 is the same as deleting since multiply all digits together
			}
		}
		
		//multiply together remaining
		int total = 1;
		for(int j: temp) {
			total *= j;
		}
		
		return total;
	}
	
	/*
	 * Generates all permuations of placements of + and *. The indices generated by this represent 
	 * the placement of the '+' sign between the i and i+1 position in the original array. 
	 * 
	 * Example 1: if [0, 2] were generated for the array {1, 2, 3, 4, 5} it 
	 * would correspond to (1 + 2) * (3 + 4) * 5. 
	 * 
	 * Example 2: generated [1, 2] for array {1, 2, 3, 4} corresponds to 1 * (2 + 3 + 4).
	 */
	private static void generatePlusIndices(ArrayList<Integer> prefix, ArrayList<Integer> suffix, int length) {
		if(prefix.size() == length) {
			Collections.sort(suffix);
			allSolutions.add(suffix);
		}
		
		for(int i = 0; i < prefix.size(); i++) {
			ArrayList<Integer> temp1 = new ArrayList<Integer>(prefix);
			ArrayList<Integer> temp2 = new ArrayList<Integer>(suffix);
			int element = prefix.get(i);
			temp1.remove(i);
			temp2.add(element);
				
			generatePlusIndices(temp1, temp2, length);
		}
	}
}