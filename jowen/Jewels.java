import java.io.File;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Scanner;
import java.util.TreeMap;

/*
 * Greedy solution to fractional Knapsack
 */
public class Jewels {
	//Max weight Ahmet can hold in his truck
	private static final int MAX_WEIGHT = 250;
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("jewels.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			int Q = f.nextInt(); f.nextLine();
			String [] names = new String[Q];
			HashMap<String, Integer> weights = new HashMap<String, Integer>();
			int [] quantities = new int[Q];
			double [] ratios = new double[Q];
			
			//input
			for(int i = 0; i<Q; i++) {
				names[i] = f.next();
				int value = f.nextInt();
				int weight = f.nextInt();
				weights.put(names[i], weight);
				ratios[i] = ((double)value)/((double)weight);
				quantities[i] = f.nextInt() * weight;
			}
			
			HashMap<String, Integer> map = fillSackWithHighest(names, ratios, quantities, new HashMap<String,Integer>(), MAX_WEIGHT);
			formatOutput(map, weights);
		}
		f.close();
	}
	
	/*
	 * Greedily chooses the item with highest value/weight ratio, removes it, then chooses the next highest until the truck is full.
	 */
	public static HashMap<String,Integer> fillSackWithHighest(String[] names, double[] ratios, int[] quantities, HashMap<String, Integer> ans, int weightLeft) {
		double maxRatio = -1;
		int index = -1;
		for(int i = 0; i<ratios.length; i++) {
			if(maxRatio < ratios[i]) {
				maxRatio = ratios[i];
				index = i;
			}
		}
		if(index < 0)
			return ans;
		ratios[index] = -1;
		int newWeightLeft = weightLeft - quantities[index];
		if(newWeightLeft > 0){
			ans.put(names[index],quantities[index]);
			fillSackWithHighest(names,ratios,quantities,ans,newWeightLeft);
		}
		else {
			ans.put(names[index], weightLeft);
		}
		return ans;
		
	}
	
	/*
	 * Output format specified by the problem
	 */
	public static void formatOutput(HashMap<String, Integer> map, HashMap<String, Integer> weights) {
		TreeMap<String, Double> output = new TreeMap<String, Double>();
		for(String name : map.keySet()) {
			output.put(name, ((double)map.get(name))/((double)weights.get(name)));
		}
		for(String name : output.keySet()) {
			System.out.printf(name +": %.2f ",output.get(name));
		}
		System.out.println();
	}


}
