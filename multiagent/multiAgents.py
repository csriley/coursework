# multiAgents.py
# --------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

from util import manhattanDistance
from game import Directions
import random, util, search

from game import Agent

class ReflexAgent(Agent):
  """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
  """


  def getAction(self, gameState):
    """
    You do not need to change this method, but you're welcome to.

    getAction chooses among the best options according to the evaluation function.

    Just like in the previous project, getAction takes a GameState and returns
    some Directions.X for some X in the set {North, South, West, East, Stop}
    """
    # Collect legal moves and successor states
    legalMoves = gameState.getLegalActions()

    # Choose one of the best actions
    scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
    chosenIndex = random.choice(bestIndices) # Pick randomly among the best

    "Add more of your code here if you want to"

    return legalMoves[chosenIndex]

  def evaluationFunction(self, currentGameState, action):
    """
    Design a better evaluation function here.

    The evaluation function takes in the current and proposed successor
    GameStates (pacman.py) and returns a number, where higher numbers are better.

    The code below extracts some useful information from the state, like the
    remaining food (oldFood) and Pacman position after moving (newPos).
    newScaredTimes holds the number of moves that each ghost will remain
    scared because of Pacman having eaten a power pellet.

    Print out these variables to see what you're getting, then combine them
    to create a masterful evaluation function.
    """
    # Useful information you can extract from a GameState (pacman.py)
    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanPosition()
    oldFood = currentGameState.getFood()
    newGhostStates = successorGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    "*** YOUR CODE HERE ***"
    foods = oldFood.asList()
    foodDist = 9999999999
    for food in foods:
      foodDist = min(foodDist, abs(newPos[0]-food[0]) + abs(newPos[1] - food[1]))
    score = 1.1/(1 + foodDist + random.randint(0,2)) 
    ghostDist = 9999999999
    for ghost in newGhostStates:
      ghostPos = ghost.getPosition()
      ghostDist = min(ghostDist, abs(newPos[0]-ghostPos[0]) + abs(newPos[1] - ghostPos[1]))
    score -= 1000 if ghostDist < 2 else 0
    return score

def scoreEvaluationFunction(currentGameState):
  """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
  """
  return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
  """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
  """

  def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
    self.index = 0 # Pacman is always agent index 0
    self.evaluationFunction = util.lookup(evalFn, globals())
    self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
  """
    Your minimax agent (question 2)
  """

  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth
      and self.evaluationFunction.

      Here are some method calls that might be useful when implementing minimax.

      gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

      Directions.STOP:
        The stop direction, which is always legal

      gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

      gameState.getNumAgents():
        Returns the total number of agents in the game
    """
    "*** YOUR CODE HERE ***"
    
    (score, action) = self.maxAction(gameState, self.depth)
    return action
      
    util.raiseNotDefined()
    
  def maxAction(self, gameState, depthLeft):
    if depthLeft > 0 and len(gameState.getLegalActions(0)) > 1:
      score = -99999999999
      act = Directions.STOP
      for action in gameState.getLegalActions(0):
        if action == Directions.STOP:
          continue
        state = gameState.generateSuccessor(0, action)
        s = 9999999999999
        s = self.minAction(state, depthLeft - 1, 1)[0]
        if s > 500000:
          s = self.evaluationFunction(state)
        if s > score:
          score = s
          act = action
      return (score, act)
    else:
      return (self.evaluationFunction(gameState), Directions.STOP)
      
  def minAction(self, gameState, depthLeft, ghost):
    ghostScore = 999999999999
    nextGhostStart = gameState
    
    for action in gameState.getLegalActions(ghost):
      state = gameState.generateSuccessor(ghost, action)
      if ghost + 1 < gameState.getNumAgents():
        state = self.minAction(state, depthLeft, ghost + 1)[1]
      s = self.maxAction(state, depthLeft)[0]
      if s < ghostScore:
        ghostScore = s
        nextGhostStart = state
    return (ghostScore, nextGhostStart)
    
class AlphaBetaAgent(MultiAgentSearchAgent):
  """
    Your minimax agent with alpha-beta pruning (question 3)
  """

  def getAction(self, gameState):
    """
      Returns the minimax action using self.depth and self.evaluationFunction
    """
    "*** YOUR CODE HERE ***"
    alphaStart = -999999999
    betaStart = 999999999
    (score, action) = self.maxAction(gameState, self.depth, alphaStart, betaStart)
    return action
    
  def maxAction(self, gameState, depthLeft, alpha, beta):
    if depthLeft > 0 and len(gameState.getLegalActions(0)) > 1:
      score = -99999999999
      act = Directions.STOP
      for action in gameState.getLegalActions(0):
        if action == Directions.STOP:
          continue
        state = gameState.generateSuccessor(0, action)
        s = self.minAction(state, depthLeft - 1, 1, alpha, beta)[0]
        if s > 50000: #Filtering out cases that don't get set
          s = self.evaluationFunction(state)
        if s > score:
          score = s
          act = action
        if score >= beta:
          return (score, act)
      if score > alpha:
        alpha = score
      return (score, act)
    else:
      return (self.evaluationFunction(gameState), Directions.STOP, alpha)
      
  def minAction(self, gameState, depthLeft, ghost, alpha, beta):
    ghostScore = 999999999999
    nextGhostStart = gameState
    
    for action in gameState.getLegalActions(ghost):
      state = gameState.generateSuccessor(ghost, action)
      if ghost + 1 < gameState.getNumAgents():
        state = self.minAction(state, depthLeft, ghost + 1, alpha, beta)[1]
      s = self.maxAction(state, depthLeft, alpha, beta)[0]
      if s < ghostScore:
        ghostScore = s
        nextGhostStart = state
      if ghostScore <= alpha:
        return (ghostScore, nextGhostStart)
      if ghostScore < beta:
        beta = ghostScore
    return (ghostScore, nextGhostStart)

class ExpectimaxAgent(MultiAgentSearchAgent):
  """
    Your expectimax agent (question 4)
  """

  def getAction(self, gameState):
    """
      Returns the expectimax action using self.depth and self.evaluationFunction

      All ghosts should be modeled as choosing uniformly at random from their
      legal moves.
    """
    "*** YOUR CODE HERE ***"
    alphaStart = -999999999
    betaStart = 999999999
    (score, action) = self.maxAction(gameState, self.depth, alphaStart, betaStart)
    return action
    
  def maxAction(self, gameState, depthLeft, alpha, beta):
    if depthLeft > 0 and len(gameState.getLegalActions(0)) > 1:
      score = -99999999999
      act = Directions.STOP
      for action in gameState.getLegalActions(0):
        if action == Directions.STOP:
          continue
        state = gameState.generateSuccessor(0, action)
        s = self.minAction(state, depthLeft - 1, 1, alpha, beta)
        if s > score:
          score = s
          act = action
        #if score >= beta:
         # return (score, act)
      if score > alpha:
        alpha = score
      return (score, act)
    else:
      return (self.evaluationFunction(gameState), Directions.STOP, alpha)
      
  def minAction(self, gameState, depthLeft, ghost, alpha, beta):
    avgScore = 0
    numActions = (float(len(gameState.getLegalActions(ghost))))
    for action in gameState.getLegalActions(ghost):
      state = gameState.generateSuccessor(ghost, action)
      if ghost + 1 < gameState.getNumAgents():
        avgScore += self.minAction(state, depthLeft, ghost + 1, alpha, beta) / numActions
      else:
        avgScore += self.maxAction(state, depthLeft, alpha, beta)[0] / numActions      
      #if avgScore <= alpha:
        #return avgScore
      if avgScore < beta:
        beta = avgScore
    if numActions == 0:
      return self.evaluationFunction(gameState)
    return avgScore 
  
def betterEvaluationFunction(currentGameState):
  """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: We consider a few factors. Among them, we use the actual score, the number of capsules eaten, the (actual) distances to the nearest ghost and nearest food, and a more complicated function involving propagating "smells" around.
  """
  "*** YOUR CODE HERE ***"
  foodList = currentGameState.getFood().asList()
  pacmanPos = currentGameState.getPacmanPosition()
  walls = currentGameState.getWalls()
  adjacents = ((-1, 0), (1, 0), (0, -1), (0, 1))
  scent = {}  
  
  #Inspired by a post on piazza regarding pheromones for ghost behavior.
  #Subsample into the food list and diffuse smell everywhere.  
  minFood = [None, None, None, None, None] #Last is closest.
  init = 9999
  minDist = [init, init, init, init, init]
  targets = foodList
  targets.extend(currentGameState.getCapsules())
  for food in targets:
    d = abs(pacmanPos[0] - food[0]) + abs(pacmanPos[1] - food[1])
    for x in range(len(minFood) - 1, -1, -1):
      if d < minDist[x]:
        begin = minFood[1:x+1]
        end = minFood[x+1:]
        minFood = begin
        minFood.append(food)
        minFood.extend(end)
        
        beginDist = minDist[1:x+1]
        endDist = minDist[x+1:]
        minDist = beginDist
        minDist.append(d)
        minDist.extend(endDist)
        break
  
  scentStep = 1
  foodScent = 3
  
  for f in minFood:
    if f is None:
      continue
    
    #Propagate a smell from each food pellet.
    toBeSmelly = [(f, foodScent)]
    while len(toBeSmelly) > 0:
      current = toBeSmelly.pop(0)
      if current[0] not in scent:
        scent[current[0]] = 0.0
      scent[current[0]] += current[1]
      for adj in adjacents:
        loc = (current[0][0] + adj[0], current[0][1] + adj[1])
        if loc not in walls and loc not in currentGameState.getGhostPositions():
          if current[1] > scentStep:
            toBeSmelly.append((loc, current[1] - scentStep))
  
  if pacmanPos not in scent:
    scent[pacmanPos] = 0
  
  #This is our smell related score.
  firstScore = (scent[pacmanPos] + currentGameState.getScore())
  
  
  #Calculate the actual closest path distance to both the nearest ghost and the nearest food.
  minDist = 0
  minGhostLen = 0
  ghostFound = False
  foodFound = False
  frontier = util.PriorityQueue()
  frontier.push((pacmanPos, 0), 0)
  explored = {}
  while not frontier.isEmpty() and (not foodFound or not ghostFound):
    current = frontier.pop()
    for adj in adjacents:
      pos = (adj[0] + current[0][0], adj[1] + current[0][1])
      if currentGameState.getFood()[pos[0]][pos[1]]:
        if minDist == 0:
          minDist = current[1] + 1
        foodFound = True
      for gpos in currentGameState.getGhostPositions():
        if gpos == pos and not ghostFound:
          minGhostLen = current[1] + 1
          ghostFound = True
      if pos not in explored and not walls[pos[0]][pos[1]]:
        frontier.push((pos, current[1] + 1), current[1] + 1)
        explored[current[0]] = True
  if currentGameState.isLose():
    minGhostLen = -99999999999999
  
  secondScore = -minDist + minGhostLen * 0.2
  return firstScore + secondScore - len(currentGameState.getCapsules()) * 50

# Abbreviation
better = betterEvaluationFunction

class ContestAgent(MultiAgentSearchAgent):
  """
    Your agent for the mini-contest
  """

  def getAction(self, gameState):
    """
      Returns an action.  You can use any method you want and search to any depth you want.
      Just remember that the mini-contest is timed, so you have to trade off speed and computation.

      Ghosts don't behave randomly anymore, but they aren't perfect either -- they'll usually
      just make a beeline straight towards Pacman (or away from him if they're scared!)
    """
    "*** YOUR CODE HERE ***"
    searcher = ExpectimaxAgent()
    searcher.evaluationFunction = better
    return searcher.getAction(gameState)

