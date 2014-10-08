import java.util.*;
import java.io.*;
public class Change_Combos
{
	/*
	output:
	3
	27
	17
	43
	311
	697
	1680
	88	
	*/
	
	
	public static void main(String [] args) throws IOException
	{
		Scanner file = new Scanner(new File("change_combos.dat"));
		int[] monetary = {47,37,23,13,5,1};
		while(file.hasNextInt())
		{
			int amount = file.nextInt();
			int count = 0;
			//Simulates the number of coins, 
			//i represents 1's coin, j represents 5's coin, 
			//k represents 13's coin, etc
			for(int i = 0; i<=200; i++)
			{
				for(int j = 0; j<=(200/5); j++)
				{
					for(int k = 0; k<=(200/13); k++)
					{
						for(int l = 0; l<=(200/23); l++)
						{
							for(int m =0; m<=(200/37); m++)
							{
								for(int n = 0; n<=(200/47); n++)
								{
									//each loop is simulated a number of coins, 
									//multiplying by their appropriate monetary values
									//will yield the value of this current combination of coins
									int value = monetary[5]*i + monetary[4]*j + monetary[3]*k + monetary[2]*l + monetary[1]*m + monetary[0]*n; 
									if(value == amount)
									{
										count++;
									}
										
								}
							}
						}
					}
				}
			}
			System.out.println(count);
		}
	}
}