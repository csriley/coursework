import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Scanner;

/*
 * Permutation generation problem 
 */

public class Chemistry {
	public static void main(String [] args) throws FileNotFoundException { 
		Scanner f = new Scanner(new File("chemistry.dat"));
		int N = f.nextInt(); f.nextLine();
		while(N-- > 0) {
			int Q = f.nextInt(); f.nextLine();
			ArrayList<String> prefix = new ArrayList<String>();
			ArrayList<String> suffix = new ArrayList<String>();
			while(Q-- > 0) {
				suffix.add(f.next());
			}
			Collections.sort(suffix);
			ArrayList<ArrayList<String>> orderedPermutations = permute(prefix, suffix);
			output(orderedPermutations);
			
		}
		f.close();		
	}
	
	//prints the given nested arraylist to console
	public static void output(ArrayList<ArrayList<String>> p) {
		int count = 1;
		for(ArrayList<String> list: p) {
			System.out.print(count + ". ");
			count++;
			for(String chemical: list) {
				System.out.print(chemical + " ");
			}
			System.out.println();
		}
		System.out.println();
	}

	/*
	 * Recursively generates all possible permutations on the given suffix until the suffix is full and returns it as 
	 * a list of list of strings.
	 */
	@SuppressWarnings("unchecked")
	public static synchronized ArrayList<ArrayList<String>> permute(ArrayList<String> prefix, ArrayList<String> suffix) {
		ArrayList<ArrayList<String>> ans = new ArrayList<ArrayList<String>>();
		if(suffix.size() == 0) {
	    	ans.add(prefix);
	    	return ans;
	    }
		
		HashSet<String> unique = new HashSet<String>();
	    for(int i = 0; i<suffix.size(); i++) {
	    	String add = suffix.get(i);
	    	if(unique.contains(add)) {
	    		continue;
	    	}
	    	
	    	unique.add(add);
	    	ArrayList<String> prefixToPass = (ArrayList<String>) prefix.clone();
	    	ArrayList<String> suffixToPass = (ArrayList<String>) suffix.clone();
	    	suffixToPass.remove(i);
	    	prefixToPass.add(add);
	    	ans.addAll(permute(prefixToPass , suffixToPass));
	    }
	    return ans;
	}
}
