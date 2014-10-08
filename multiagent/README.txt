csr726, crystal.sno.riley@gmail.com
wd3282, googlyeyesultra@gmail.com

We pair programmed the entire assignment; we swapped drivers regularly, often times in the middle of methods.

We did run into an issue when attempting to implement AlphaBeta Pruning. However, we re-read the text and looked up the implementation, then had no issues.

Our solution to betterEvaluationFunction involves the mentioned 'pheromone' tactic. Basically, we find several nearby food (or capsules) and propagate a scent from them (decreasing scent value as 1 per tile away from the food). This scent will be blocked by ghosts or walls. We prefer tiles with higher scent values.  

Our contest submission calls Expectimax agent. 
