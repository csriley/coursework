import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.TreeMap;


public class Punch {
	
	public static String score = "";
	public static TreeMap<Integer, ArrayList<Character>> map = new TreeMap<Integer, ArrayList<Character>>();
	
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("punch.dat"));
		while(f.hasNextLine()) {
			String line = f.nextLine();
			Scanner in = new Scanner(line);
			String [] card = new String[9];
			
			//create card
			for(int i = 0; i<9; i++) {
				String code = in.next();
				card[i] = createCardEntry(code);
			}
			
			//initialize for each iteration
			score = "";
			for(int i = 0; i<6; i++) {
				map.put(i, new ArrayList<Character>());
			}
			
			//process the card
			processCard(card);
			
			//pull info from map to String
			String name = formatName();
			
			System.out.println(score + " "+ name);
		}
	}
	
	/*
	 * Creates an array of Strings representing the cards bubbled values as '1's
	 */
	public static String createCardEntry(String code) {
		String val = "";
		for(int x = 0; x<3; x++) {
			int hex = Integer.parseInt(code.substring(x,x+1),16);
			StringBuffer temp = new StringBuffer(String.format("%4s",Integer.toBinaryString(hex)));
			temp.reverse();
			val += temp.toString();
		}
		return val;
	}
	
	/*
	 * Parses the number in the score and maps each bubble to a letter (keeps track of duplicates)
	 */
	public static void processCard(String [] card) {
		for(int i =0; i<9; i++) {
			card[i] = card[i].replaceAll(" ","0");
			int eachScore = 9-card[i].indexOf("1");
			int count = card[i].length() - card[i].replaceAll("1","").length();
			if(i < 3) {
				if(count != 1 || eachScore < 0) {
					score += "*";
				}
				else
					score += eachScore;
			}
			else {
				String temp = card[i];
				while(temp.indexOf("1") >= 0) {
					map.get(temp.indexOf("1")).add((char)('A' + (i-3)));
					temp = temp.substring(0, temp.indexOf("1")) + "0" + temp.substring(temp.indexOf("1") +1);
				}
			}
		}
	}
	
	/*
	 * Replaces dupilcate entries with '*'s and no entries with '*'s and
	 * removes '*'s from the end of the name and returns a string of the 
	 * formatted name.
	 */
	public static String formatName() {
		String name = "";
		for(int index: map.keySet()) {
			ArrayList<Character> temp = map.get(index);
			if(temp.size() != 1)
				name += '*';
			else
				name += temp.get(0);
		}
		name = name.substring(0,1) + ". " + name.substring(1);
		while(name.endsWith("*")) {
			name = name.substring(0,name.length()-1);
		}
		return name;
	}
}
