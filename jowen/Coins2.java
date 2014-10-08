import java.io.File;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Scanner;
import java.util.TreeMap;

//Dynamic Programming using memoization solution for making change of arbitrary denominations

public class Coins {
	public static boolean DEBUG = false;
	public static void main(String [] args) throws FileNotFoundException {
		
		Scanner f = new Scanner(new File("coins.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			//reset values from previous iterations
			solutionBucket = new HashMap<Integer, Tup>();
			bestAmount = Integer.MAX_VALUE;
			
			//grab input
			int coins = f.nextInt();
			int [] denominations = new int[coins];
			String [] names = new String[coins];
			HashMap<String, Integer> occurences = new HashMap<String, Integer>();
			
			for(int i = 0; i < coins; i++) {
				names[i] = f.next();
				denominations[i] = f.nextInt();
				occurences.put(names[i], 0);
			}
			int amount = f.nextInt();
			
			//start solution building
			makeChange(denominations, names, amount, occurences, 0);
			if(DEBUG) {
				System.out.println(solutionBucket);
			} else {
				processOutput(solutionBucket.get(0).left); 
			}
		}
		f.close();
	}
	
	//Stores previous sub problems indexed by their total value from max so (max - cur)
	private static HashMap<Integer, Tup> solutionBucket= new HashMap<Integer, Tup>();
	
	//the smallest number of coins that is a solution we have seen so far
	private static int bestAmount = Integer.MAX_VALUE;
	
	/*
	 * Recursively adds coins and builds a solution.
	 */
	public static void makeChange(int [] values, String [] names, int curAmount, HashMap<String, Integer> soFar, int numberOfCoins) {
		
		//this isn't a solution
		if(curAmount < 0 || numberOfCoins > bestAmount) {
			return;
		}

		//if we have computed this subproblem we should check if we have created something better
		//otherwise add this to subproblem collection
		if(solutionBucket.containsKey(curAmount)) {
			Tup tup = solutionBucket.get(curAmount);
			
			//is the solution to this sub problem better, 
			//if so, change the value of this subproblem to the better one
			//other wise, no need to continue with this solution
			if(tup.right > numberOfCoins) {
				tup.right = numberOfCoins;
				tup.left = soFar;
			} else {
				return;
			}
		} else {
			solutionBucket.put(curAmount, new Tup(soFar, numberOfCoins));
		}
		
		//we have found a solution! update the least number of coins
		if(curAmount == 0 && numberOfCoins < bestAmount) {
			bestAmount = numberOfCoins;
		}
		
		//Add a coin to this solution and call again 
		for(int i = 0; i < values.length; i++) {
			//This map contains the coin names as keys and the number of times we have added each coins as values
			HashMap<String, Integer> soFar2 = new HashMap<String,Integer>(soFar);
			soFar2.put(names[i], soFar.get(names[i]) + 1);
			
			makeChange(values, names, curAmount - values[i], soFar2, numberOfCoins + 1);
		}
	}
	
	/*
	 * Formats output
	 */
	public static void processOutput(HashMap<String, Integer> occurrences) {
		TreeMap<String, Integer> map = new TreeMap<String, Integer>(occurrences);
		for(String a : map.keySet()) {
			if(map.get(a) > 0) {
				System.out.print(a +": " + map.get(a) + " ");
			}
		}
		System.out.println();
	}
}

/*
 * Java doesn't have tuples and this makes me angry
 */
class Tup {
	
	public HashMap<String,Integer> left;
	public int right;
	
	public Tup(HashMap<String,Integer> map, int in) {
		left = map;
		right = in;
	}
	public String toString() {
		return "(" + left + ", " + right + ")";
	}
}
