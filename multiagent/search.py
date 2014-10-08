# search.py
# ---------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

"""
In search.py, you will implement generic search algorithms which are called 
by Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
  """
  This class outlines the structure of a search problem, but doesn't implement
  any of the methods (in object-oriented terminology: an abstract class).
  
  You do not need to change anything in this class, ever.
  """
  
  def getStartState(self):
     """
     Returns the start state for the search problem 
     """
     util.raiseNotDefined()
    
  def isGoalState(self, state):
     """
       state: Search state
    
     Returns True if and only if the state is a valid goal state
     """
     util.raiseNotDefined()

  def getSuccessors(self, state):
     """
       state: Search state
     
     For a given state, this should return a list of triples, 
     (successor, action, stepCost), where 'successor' is a 
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental 
     cost of expanding to that successor
     """
     util.raiseNotDefined()

  def getCostOfActions(self, actions):
     """
      actions: A list of actions to take
 
     This method returns the total cost of a particular sequence of actions.  The sequence must
     be composed of legal moves
     """
     util.raiseNotDefined()
           

def tinyMazeSearch(problem):
  """
  Returns a sequence of moves that solves tinyMaze.  For any other
  maze, the sequence of moves will be incorrect, so only use this for tinyMaze
  """
  from game import Directions
  s = Directions.SOUTH
  w = Directions.WEST
  return  [s,s,w,s,w,w,s,w]

def depthFirstSearch(problem):
  """
  Search the deepest nodes in the search tree first [p 85].
  
  Your search algorithm needs to return a list of actions that reaches
  the goal.  Make sure to implement a graph search algorithm [Fig. 3.7].
  
  To get started, you might want to try some of these simple commands to
  understand the search problem that is being passed in:
  
  print "Start:", problem.getStartState()
  print "Is the start a goal?", problem.isGoalState(problem.getStartState())
  print "Start's successors:", problem.getSuccessors(problem.getStartState())
  """
  "*** YOUR CODE HERE ***"
  #Recursive DFS calls helper doDFS, if goal state is start state then we return without going to doDFS
  state = problem.getStartState()
  if(problem.isGoalState(state)):
    return []
  else:
    actions = doDepthFirstSearch(problem, state, None, {})
    #This is reversed because we build the action list backwards, starting from the goal state and working backwards. This reverse is necessary to return the actions in the correct order.
    actions.reverse()
    return actions
  
#doDFS looks for the goal state, once the goal state is found, actions are appended by the recursive upcalls
def doDepthFirstSearch(problem, current, action, visited):
  if current in visited:
    return None
  
  if(problem.isGoalState(current)):
    return [action]
  
  visited[current] = True
  successors = problem.getSuccessors(current)
  for successor in successors:
    result = doDepthFirstSearch(problem, successor[0], successor[1], visited)
    if result is not None:
      if action is not None:
        result.append(action)
    
      return result
  return None
   

#We keep a list of states to expand, along with their action chains, and a hash of visited states.
def breadthFirstSearch(problem):
  "Search the shallowest nodes in the search tree first. [p 81]"
  "*** YOUR CODE HERE ***"
  
  start = problem.getStartState()
  nodes = [(start, [])]
  visited = {start:True}
  if problem.isGoalState(start):
      return [] #Start is goal.
  while len(nodes) > 0:
    current = nodes.pop(0)
    successors = problem.getSuccessors(current[0])
    for s in successors: #Add children.
      actions = current[1][:]
      actions.append(s[1])
      if problem.isGoalState(s[0]): #As per the book, check when we generate for time complexity.
        return actions
      if(s[0] not in visited): #Don't reexpand nodes.
        nodes.append((s[0], actions))
        visited[s[0]] = True
      
def uniformCostSearch(problem):
  "Search the node of least total cost first. "
  "*** YOUR CODE HERE ***"
  
  #Exactly like BFS except we maintain a priority queue rather than a list of nodes to expand.
  start = problem.getStartState()
  nodes = util.PriorityQueue()
  nodes.push((start, []), problem.getCostOfActions([]))
  visited = {start:True}
  if problem.isGoalState(start):
      return [] #Start is goal.
  while not nodes.isEmpty():
    current = nodes.pop()
    successors = problem.getSuccessors(current[0])
    for s in successors: #Add children.
      actions = current[1][:]
      actions.append(s[1])
      if problem.isGoalState(s[0]): #As per the book, check when we generate for time complexity.
        return actions
      if(s[0] not in visited): #Don't reexpand nodes.
        nodes.push((s[0], actions), problem.getCostOfActions(actions))
        visited[s[0]] = True

def nullHeuristic(state, problem=None):
  """
  A heuristic function estimates the cost from the current state to the nearest
  goal in the provided SearchProblem.  This heuristic is trivial.
  """
  return 0

def aStarSearch(problem, heuristic=nullHeuristic):
  "Search the node that has the lowest combined cost and heuristic first."
  "*** YOUR CODE HERE ***"
  #Exactly like UCS except we add the heuristic.
  start = problem.getStartState()
  nodes = util.PriorityQueue()
  nodes.push((start, []), problem.getCostOfActions([]) + heuristic(start, problem))
  visited = {start:True}
  if problem.isGoalState(start):
      return [] #Start is goal.
  while not nodes.isEmpty():
    current = nodes.pop()
    successors = problem.getSuccessors(current[0])
    for s in successors: #Add children.
      actions = current[1][:]
      actions.append(s[1])
      if problem.isGoalState(s[0]): #As per the book, check when we generate for time complexity.
        return actions
      if(s[0] not in visited): #Don't reexpand nodes.
        nodes.push((s[0], actions), problem.getCostOfActions(actions) + heuristic(current[0], problem))
        visited[s[0]] = True
    
  
# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
