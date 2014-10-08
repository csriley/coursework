import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.TreeSet;

public class Bad {
	public static TreeSet<String> symbols;
	public static void main(String [] args) throws FileNotFoundException {
		Scanner elements = new Scanner(new File("elements.dat"));
		symbols = new TreeSet<String>();
		while(elements.hasNext()) {
			elements.next(); //atomic number
			elements.next(); //dash
			symbols.add(elements.next().toLowerCase()); //symbol
			elements.next(); //dash
			elements.next(); //element name
		}
		elements.close();
		Scanner f = new Scanner(new File("bad.dat"));
		while(f.hasNext()) {
			String firstName = f.next();
			System.out.print(processString(firstName.toLowerCase())+ " ");
			String lastName = f.next();
			System.out.println(processString(lastName.toLowerCase()));
		}
	}
	
	public static String processString(String name) {
		for(String symbol: symbols) {
			if(name.contains(symbol)) {
				int index = name.indexOf(symbol);
				name = name.substring(0, index) + symbol.substring(0,1).toUpperCase() + symbol.substring(1) + "_" + name.substring(index + symbol.length());
				break;
			}
		}
		return name.substring(0,1).toUpperCase() + name.substring(1);
	}
	
}
