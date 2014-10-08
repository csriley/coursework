package problems2014;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;

/**
 * Given a graph of friends find out who to recommend as a friend to a given person as
 * specified as the friend of a friend whose set of friends has the largest intersection 
 * with your own 
 */
public class Friends {
	public static void main(String [] args) throws FileNotFoundException {
		Map<String, Friend> globalFriends = new HashMap<String, Friend>();
		Scanner f = new Scanner(new File("/Users/sno/Documents/workspace/JOwen/src/problems2014/friends.dat"));
		while(f.hasNextLine()) {
			Scanner chop = new Scanner(f.nextLine());
			String first = chop.next();
			if(first.charAt(0) == '-') {
				break;
			}
			first = first.substring(0, first.length()-1);
			globalFriends.put(first, new Friend(first));
			while(chop.hasNext()) {
				globalFriends.get(first).listOfFriends.add(chop.next());
			}
		}
		
		fillFriendGraphFromGlobalStore(globalFriends);
		
		while(f.hasNextLine()) {
			Friend my = globalFriends.get(f.next());
			Friend bestFriendOfFriend = findBestFriendOfFriend(my);
			System.out.println(bestFriendOfFriend.name);
		}
	}
	
	/*
	 * The global store keeps a map of the names of the friends to their instances. The
	 * friend graph is populated by taking these instances and looking at the listOfFriends 
	 * that each friend have and populating the set 'friends' with the actual instances.
	 */
	private static void fillFriendGraphFromGlobalStore(Map<String, Friend> globalStore) {
		for(String name: globalStore.keySet()) {
			globalStore.get(name).friends = new HashSet<Friend>();
			for(String friendsName: globalStore.get(name).listOfFriends) {
				globalStore.get(name).friends.add(globalStore.get(friendsName));
			}
		}
	}
	
	/*
	 * Returns the friend of friend that has the most friends in common with 
	 * the given friend.
	 */
	private static Friend findBestFriendOfFriend(Friend my) {
		Friend bestFriendOfFriend = null;
		int amount = -1;
		for(Friend friend: my.friends) {
			for(Friend friendOfFriend: friend.friends) {
				if(friendOfFriend.equals(my) || my.friends.contains(friendOfFriend)) {
					continue;
				}
				int tempAmount = intersectionSize(my.friends, friendOfFriend.friends);
				if(amount < tempAmount) {
					bestFriendOfFriend = friendOfFriend;
					amount = tempAmount;
				}
			}
		}
		return bestFriendOfFriend;
	}
	
	/*
	 * Returns the number of elements common to both sets
	 */
	private static int intersectionSize(Set<Friend> a, Set<Friend> b) {
		int count = 0;
		for(Friend friend: a) {
			if(b.contains(friend)) {
				count++;
			}
		}
		
		return count;
	}
}

class Friend {
	public Set<Friend> friends; //set of friends that are friends with this friend
	public ArrayList<String> listOfFriends; //the arraylist of strings that are friends with this friend
	public String name; //the name of this friend
	public Friend(String name) {
		this.name = name;
		this.listOfFriends = new ArrayList<String>();
	}
}
