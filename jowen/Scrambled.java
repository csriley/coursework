package problems2014;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.Stack;

/**
 * Given a prefix notation for + and - return the result of the operation
 */
public class Scrambled {
	public static void main(String [] args) throws FileNotFoundException {
		Scanner f = new Scanner(new File("/Users/sno/Documents/workspace/JOwen/src/problems2014/scrambled.dat"));
		while(f.hasNextLine()) {
			String input = f.nextLine();
			System.out.println(compute(getOperations(input), getOperands(input)));
		}
	}
	
	/*
	 * Returns the stack of operations given a string calculation in prefix notation
	 */
	private static Stack<Character> getOperations(String input) {
		Stack<Character> operators = new Stack<Character>();
		Scanner chop = new Scanner(input);
		while(chop.hasNext()) {
			char next = chop.next().charAt(0);
			if(next != '-' && next != '+') {
				break;
			}
			operators.push(next);
		}
		
		return operators;
	}
	
	/*
	 * Returns the stack of operands (inputted from right to left)
	 */
	private static Stack<Integer> getOperands(String input) {
		int lastIndexMinus = input.lastIndexOf("-");
		int lastIndexPlus = input.lastIndexOf("+");
		//location of the farthers rightmost operator +2 is the beginning of the numbers.
		int startNumIndex = lastIndexMinus > lastIndexPlus ? lastIndexMinus + 2: lastIndexPlus + 2; 
		String[] numbers = input.substring(startNumIndex).split(" "); 
		Stack<Integer> operands = new Stack<Integer>();
		for(int i = numbers.length -1 ; i >= 0; i--) {
			operands.push(Integer.parseInt(numbers[i]));
		}
		
		return operands;
	}
	
	/*
	 * Returns the value of a calculation where the operators and operands are in corresponding stacks. 
	 */
	private static int compute(Stack<Character> operators, Stack<Integer> operands) {
		while(!operators.isEmpty()) {
			int left = operands.pop();
			int right = operands.pop();
			char op = operators.pop();
			if(op == '+') {
				operands.push(left + right);
			}
			
			if(op == '-') {
				operands.push(left - right);
			}
		}
		
		//there should only be one value left in this stack when the operators stack is empty
		return operands.pop(); 
	}
}
