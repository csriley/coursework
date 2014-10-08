import java.util.*;
import java.io.*;
public class Coins
{
	public static void main(String[] args) throws Exception
	{
		Scanner file = new Scanner(new File("change_combos.dat"));
		/*int nNumberOfSets = file.nextInt();
		for (int nOuterLoop = 0; nOuterLoop < nNumberOfSets; nOuterLoop++)
		{
			int nTimesToUse = file.nextInt();
			file.nextLine();
			String[] lCoinValueStrings = file.nextLine().trim().split("\\s+");
			Integer[] lCoins = new Integer[lCoinValueStrings.length];
			for (int i = 0; i < lCoins.length; i++)
			{
				lCoins[i] = Integer.parseInt(lCoinValueStrings[i]);
			}
			Arrays.sort(lCoins, new Comparator<Integer>(){
				@Override
				public int compare(Integer one, Integer two)
				{
					return -1 * one.compareTo(two);
				}
			});*/
			Integer[] lCoins = {47,37,23,13,5,1};
			for (int i = 0; file.hasNext(); i++)
			{
				int nValue = file.nextInt();
				System.out.printf("%d", r(nValue, lCoins));
				System.out.println();
			}
			System.out.println();
		//}
	}
	private static int r(int n, Integer[] arr)
	{
		int[] lCoins = new int[arr.length];
		for (int i = 0; i < arr.length; i++)
		{
			lCoins[i] = arr[i];
		}
		return r(0,true, 0, n, lCoins);
	}
	private static int r(int index, boolean first, int current, final int target, final int[] coins)
	{
		if (index >= coins.length)
		{
			return 0;
		}
		if (current > target)
		{
			return 0;
		}
		if (current == target)
		{
			return 1;
		}
		int ways = r(index + 1, true, current, target, coins);
		while (current < target && first)
		{
			ways += r(index, false, current += coins[index], target, coins);
		}
		return ways;
	}
}
