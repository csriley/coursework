import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Scanner;
import java.util.TreeMap;

// Dynamic Programming Solution to 0-1 Knapsack.

public class Knapsack {
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("knapsack.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			int maxWeight = f.nextInt();f.nextLine();
			int items = f.nextInt(); f.nextLine();
			ArrayList<String> names = new ArrayList<String>();
			ArrayList<Integer> values = new ArrayList<Integer>();
			ArrayList<Integer> weights = new ArrayList<Integer>();
			//In normal DP, the first row of the matrix is filled with zeros. To avoid with indexing and zeros, with
			// KnapObj, the first element of each list is a dummy that will be ignored. This is algorithmically 
			// equivalent to padding the dp matrix with zeros
			names.add(null);
			values.add(0);
			weights.add(Integer.MAX_VALUE);
			for(int i = 0; i<items; i++) {
				//String name = f.next();
				//int quantity = f.nextInt();
				int value = f.nextInt();
				int weight = f.nextInt();
				
				//does the reduction from bounded to 0-1 knapsack by introducing multiple identical items
				//based up quantity. This does lead to a slower solution, and is not scalable to large data sets.
				//for(int j = 0; j<quantity; j++) {
					names.add("Item #" + (i + 1));
					values.add(value);
					weights.add(weight);
				//}
			}
			
			if(maxWeight == 0) {
				System.out.println(transformOutput("", names, maxWeight, weights, values));
				continue;
			}
			String ans = knapsack(values, weights, names, names.size(), maxWeight);
			System.out.println(transformOutput(ans, names, maxWeight, weights, values));
		}
	}
	
	/*
	 * Each element m[i][j] contains a solution to the subproblem of including the first i-1 elements (in n) and the lesser weight W. 
	 * From this subproblem, the addition of an item i is just the added weight of i and if this is an increase in value, then it should
	 * be recorded. 
	 */
	public static String knapsack(ArrayList<Integer> v, ArrayList<Integer> w, ArrayList<String> n, int q, int W) { 
		KnapsackObj [][] m = new KnapsackObj[q][W+1];
		
		for(int j = 0; j<m[0].length; j++) {
			m[0][j] = new KnapsackObj("", 0);
		}
		
		for(int i = 1; i<m.length; i++) {
			for(int j = 0; j<m[0].length; j++) {
				//To keep from double counting, we look at only subproblems where this ith item couldn't be located
				if(j >= w.get(i)) {
					//does adding this item increase this subproblems value?
					if(m[i-1][j].maxV < m[i-1][j-w.get(i)].maxV+v.get(i)) {
						//add the item and update the value
						m[i][j] = new KnapsackObj(m[i-1][j-w.get(i)].items + " " + n.get(i), m[i-1][j-w.get(i)].maxV+v.get(i));
					}
					else {
						//keep the old value
						m[i][j] = new KnapsackObj(m[i-1][j].items, m[i-1][j].maxV);
					}
				}
				else {
					//keep the value of the last iteration
					m[i][j] = new KnapsackObj(m[i-1][j].items, m[i-1][j].maxV);
				}
			}
		}
		
		//Debugging
		/*for(int i = 0; i<q; i++) {
			for(int j = 0; j<m[0].length; j++) {
				System.out.print(m[i][j] + " ");
			}System.out.println();
		}*/
		
		//this solution has the best solutions of subproblems of lesser weights and various combinations 
		//items, so will therefore be the best solution of all items and entire weight.
		return m[q-1][W].items;
	}
	public static String transformOutput(String ans, ArrayList<String> items, int maxWeight, ArrayList<Integer> weights, ArrayList<Integer> values) {
		items.remove(0);
		weights.remove(0);
		values.remove(0);
		TreeMap<String, String> out = new TreeMap<String, String>();
		HashSet<String> set = new HashSet<String>(items);
		int totalWeight = 0;
		int totalValue = 0;
		for(String item: set) {					
			if(ans.contains(item)) {
				totalWeight += weights.get(items.indexOf(item));
				totalValue += values.get(items.indexOf(item));
				out.put(item, "v=" + values.get(items.indexOf(item))+":w=" + weights.get(items.indexOf(item)));
			}
		}
		
		String ret = "knapsack capacity = " + maxWeight + "\n";
		ret += "total weight = " + totalWeight + "\n";
		ret += "total value = " + totalValue + "\n";
		for(String item: out.keySet()) {
			ret+=item+" -  " + out.get(item) + "\n";
		}
		return ret;
	}
}

//this class is convenient for keeping up with components of solution.
class KnapsackObj {
	public int maxV;
	public int curW;
	public String items;
	public KnapsackObj(String items, int maxV) {
		this.maxV = maxV;
		this.items = items;
	}
	
	public String toString() {
		return items + ": " + maxV;
	}
}