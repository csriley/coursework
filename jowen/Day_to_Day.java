import java.util.*;
import java.io.*;
public class Day_to_Day
{
	//sample output, wrong order
	/*
	sample output:
	31.0
	5.285714285714286
	-5.999999999999999
	-2.9639156081756486
	additional output
	30.0
	4.909090909090909
	-8.0
	-0.6140650784460308
	*/
	public static void main(String [] args) throws IOException
	{
		Scanner file = new Scanner(new File("day_to_day.dat"));
		double a = file.nextDouble();
		double b = file.nextDouble();
		double c = file.nextDouble();
		double d = file.nextDouble();
		System.out.println(crash(a,b));
		System.out.println(dash(a,b));
		System.out.println(mash(a,b));
		System.out.println(trash(a,b,c,d));
	}
	public static double crash(double a, double b)
	{
		return (a/4.0) + (7*b);
	}
	public static double dash(double a, double b)
	{
		return (a+Math.pow(b,2))/(a+(1.0/b))+(a/b);
	}
	public static double mash(double a, double b)
	{
		return (1.0/(1.0/a - 1.0/b));
	}
	public static double trash(double a, double b, double c, double d)
	{
		return (4.0/a/b)*Math.sqrt((1+(5.0/(c+d)))/a) - a/(c+d);
	}
}