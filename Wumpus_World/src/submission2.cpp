// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Abdullah Younis
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include <cstdlib>
#include "MyAI.hpp"
#include <time.h> 
#include <iostream>
using namespace std;


MyAI::MyAI() : Agent()
{


	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	//Initialize actionsQueue params
	front = 0;
	rear = -1;
	qSize = 0;

	//Initialize retreatStack params
	top = -1;
	sSize = 0;

	//Set initial direction of agent
	directionFacing = 'r';
	//Set intitial position of agent
	currentRow = 0;
	currentCol = 0;

	thresholdScore = 0;
	//
	arrowUsed = false;
	wumpusDead = false;
	gotTheGold = false;

	//
	moves = 0;
	wumpusCost = 1000;
	pitCost = 1000;
	worldDimensionX = 7;
	worldDimensionY = 7;
	strategy = 1;//By defualt, strategy is gold search

	//Initialize maps
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (i == 0 && j == 0) {
				//For start square, mark it as visited (1)
				visitedMap[i][j] = 1;
			}
			else {
				visitedMap[i][j] = 0;
			}

			//wumpusMap[i][j] = 0.0;
			//pitMap[i][j] = 0.0;
		}
	}

	//Initialize KB
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (i == 0 && j == 0) {
				//Start square is safe
				//MyAI::SquareKb skb(true, false, false, false);
				SquareKB skb(true, false, false, false);
				KB[i][j] = skb;
			}
			else {
				//All other squares are unknown
				//MyAI::SquareKB skb(false, false, false, false);
				SquareKB skb(false, false, false, false);
				KB[i][j] = skb;
			}
		}
	}

	//Initialize retreat stack
	//MyAI::Cell c(0, 0);
	Cell c(0, 0);
	push(c);


	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}

//MyAI is an extension of agent
Agent::Action MyAI::getAction
(
	bool stench,
	bool breeze,
	bool glitter,
	bool bump,
	bool scream
)
{
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================


	if (actionsQueueEmpty()) {//No action queued up, so determine next action(s)
		cout << "actionsQueue empty\n";
		if (strategy == 1) {//Looking for gold
			cout << "looking for gold\n";
			//cout << "preparing next move. Moves = " << moves << " col " << currentCol << " row " << currentRow << "\n";
			if (moves > 0 && currentCol == 0 && currentRow == 0) {//If agent finds itself in (0,0)
				//cout << "can climb\n";
				return CLIMB;
			}

			else {
				//cout << "can't climb\n";
			}


			if (glitter) {
				strategy = -1;//Retreat mode
				gotTheGold = true;
				return GRAB;
				//Get the hell out!
			}
			else if (bump) {
				/*if (currentCol > currentRow) {
					worldDimension = currentCol + 1;
				}
				else {
					worldDimension = currentRow + 1;
				}*/

				if (directionFacing == 'r') {
					worldDimensionX = currentCol;//currentCol is the x comp of square wrongly pushed to stack. Note that stack hasn't yet been popped
				}

				if (directionFacing == 'u') {
					worldDimensionY = currentRow;//currentRow is the y comp of square wrongly pushed to stack. Note that stack hasn't yet been popped
				}
				cout << "bump detected. Dimensions are now X: " << worldDimensionX << " Y: " << worldDimensionY << "\n";
				//Pop the stack
				pop();
				//Get new correct coordinates
				currentCol = peak().col;
				currentRow = peak().row;
			}
			else if (stench) {
				/*if (!wumpusDead && !arrowUsed)
					//Try to detect Wumpus position using KB inferences and if certainly is above a certain threshold, turn in its direction and shoot
					arrowUsed = true;
					return SHOOT;*/

					//Update KB
				KB[currentCol][currentRow].stench = true;
				//Update maps
				//updateMaps(wumpusMap, currentCol, currentRow);

			}
			else if (breeze) {

				//Update KB
				KB[currentCol][currentRow].breeze = true;
				//Update maps
				//updateMaps(pitMap, currentCol, currentRow);
			}
			else if (scream) {
				wumpusDead = true;
				//Remove stenches
				for (int i = 0; i < 7; i++) {
					for (int j = 0; j < 7; j++) {
						//wumpusMap[i][j] = 0;
						if (KB[i][j].stench) {
							KB[i][j].stench = false;
						}
					}
				}

			}

			//Choose best action(s) and fill the actions queue.
			return chooseAction(currentCol, currentRow);

			//Move there
			//Move forward until hit bump
			//return actions[rand() % 6];
		}
		else if (strategy == -1) {//Retreating
			cout << "retreating\n";

			cout << "implementing retreat from actions (" << currentCol << ", " << currentRow << ")\n";
			if (currentCol == 0 && currentRow == 0) {//If agent finds itself in (0,0)
				cout << "must climb\n";
				return CLIMB;
			}
			cout << "cannot climb :(\n";

			if (gotTheGold) {
				cout << "Got gold. Keep running!\n";
				return retreat();
			}
			else {
				cout << "Was in retreat mode but no gold, so go to choose action\n";
				//First check if there is any potential safe unvisited square in which case we change strategy back to 1 
				//Otherwise keep retreating(Done in chooseAction() function)
				return chooseAction(currentCol, currentRow);

			}

		}

	}
	else {
		cout << "actions q still has " << qSize << " move(1)\n";
		return dequeue();//Return action at front of queue
	}



	//return CLIMB;
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================
//MyAI::Cell::Cell(int c, int r) :col(c), row(r) {}
Cell::Cell() :col(0), row(0) {}
Cell::Cell(int c, int r) : col(c), row(r) {}

//MyAI::SquareKB::SquareKB(bool sa, bool br, bool st, bool gl):safe(sa), breeze(br), stench(st), gold(gl){}
SquareKB::SquareKB() : safe(false), breeze(false), stench(false), gold(false) {}
SquareKB::SquareKB(bool sa, bool br, bool st, bool gl) : safe(sa), breeze(br), stench(st), gold(gl) {}


bool MyAI::retreatStackEmpty() {
	return (top < 0);
}


//void MyAI::push(MyAI::Cell c) {
void MyAI::push(Cell c) {
	top++;
	sSize++;
	retreatStack[top] = c;
}

//MyAI::Cell MyAI::pop() {
void MyAI::pop() {
	if (!retreatStackEmpty()) {
		cout << "TOS was (" << retreatStack[top].col << ", " << retreatStack[top].row << ")\n";
		top--;
		sSize--;
		cout << "TOS now (" << retreatStack[top].col << ", " << retreatStack[top].row << ")\n";
	}
}

Cell MyAI::peak() {
	if (!retreatStackEmpty()) {
		return retreatStack[top];
	}
}

Agent::Action MyAI::dequeue() {
	if (!actionsQueueEmpty()) {
		int oldFront = front;
		front = (front + 1) % 200;
		qSize--;

		//Update direction if action is a turn before returning
		if (actionsQueue[oldFront] == TURN_LEFT || actionsQueue[oldFront] == TURN_RIGHT) {
			updateDirectionFacing(actionsQueue[oldFront]);
		}


		if (actionsQueue[oldFront] == TURN_LEFT) {
			cout << "turning left\n";
		}
		else if (actionsQueue[oldFront] == TURN_RIGHT) {
			cout << "turning right\n";
		}

		if (actionsQueue[oldFront] == FORWARD) {
			cout << "moving forward\n";
			moves++;
		}
		return actionsQueue[oldFront];
	}
}

void MyAI::enqueue(Action act) {
	if (!actionsQueueFull()) {
		rear = (rear + 1) % 200;
		actionsQueue[rear] = act;
		qSize++;
	}
}

bool MyAI::actionsQueueEmpty() {
	return qSize == 0;
}

bool MyAI::actionsQueueFull() {
	return qSize == 200;
}



float MyAI::determineSquareCost(int x, int y, char squarePositionRelativeToAgent) {

	/*for (int i = 0; i < worldDimensionX; i++) {

		for (int j = 0; j < worldDimensionY; j++) {



		}

	}*/

	//
	int thisX = x;
	int thisY = y;
	int agentX;
	int agentY;
	float probWumpus;
	float probBreeze;
	float finalScore = 0.0;

	if (squarePositionRelativeToAgent == 't') {
		agentX = x;
		agentY = y - 1;
	}
	else if (squarePositionRelativeToAgent == 'b') {
		agentX = x;
		agentY = y + 1;
	}
	else if (squarePositionRelativeToAgent == 'l') {
		agentX = x + 1;
		agentY = y;
	}
	else if (squarePositionRelativeToAgent == 'r') {
		agentX = x - 1;
		agentY = y;
	}

	//We can assume square is completely safe if there is no percept where agent currently is
	if (!KB[agentX][agentY].breeze && !KB[agentX][agentY].stench) {
		//No stench or no breeze where agent is
		finalScore += 0.0;
	}
	else if (KB[agentX][agentY].breeze) {
		if (!KB[x][y].safe) {//If current square we are considering had not been previously deemed safe
			//Infer probability by looking at rest of KB squares in relation to this one
			probBreeze = 0.25;
			finalScore += probBreeze * pitCost;
		}

	}
	else if (KB[agentX][agentY].stench) {
		if (!KB[x][y].safe) {//If current square we are considering had not been previously deemed safe
			//Infer probability by looking at rest of KB squares in relation to this one
			probWumpus = 0.25;
			finalScore += probWumpus * wumpusCost;
		}

	}
	else {
		cout << "This condition in determining score wasn't reached\n";
	}


	return finalScore;

}


void MyAI::updateDirectionFacing(Action directionTurned) {
	if (directionFacing == 'r') {
		if (directionTurned == TURN_LEFT) {
			directionFacing = 'u';
		}
		else if (directionTurned == TURN_RIGHT) {
			directionFacing = 'd';
		}
	}
	else if (directionFacing == 'u') {
		if (directionTurned == TURN_LEFT) {
			directionFacing = 'l';
		}
		else if (directionTurned == TURN_RIGHT) {
			directionFacing = 'r';
		}
	}
	else if (directionFacing == 'l') {
		if (directionTurned == TURN_LEFT) {
			directionFacing = 'd';
		}
		else if (directionTurned == TURN_RIGHT) {
			directionFacing = 'u';
		}
	}
	else if (directionFacing == 'd') {
		if (directionTurned == TURN_LEFT) {
			directionFacing = 'r';
		}
		else if (directionTurned == TURN_RIGHT) {
			directionFacing = 'l';
		}
	}
}

bool MyAI::isValidSquare(int x, int y) {
	return (x >= 0 && x <= worldDimensionX - 1 && y >= 0 && y <= worldDimensionY - 1);
}



Agent::Action MyAI::chooseAction(int x, int y) {
	cout << "Implementing choose action\n";

	srand(time(NULL));

	int chosenCol;
	int chosenRow;
	int chosenSquareIndex;
	int bestScoreSoFar = 0;
	bool to = false;//Top square exists
	bool bo = false;
	bool le = false;
	bool ri = false;
	float topSquareScore;
	float bottomSquareScore;
	float leftSquareScore;
	float rightSquareScore;
	//float uptimalSquareCost = 100000000.0;
	//bool squareChosen = false;


	if (isValidSquare(x, y + 1)) {//Bounds check
		topSquareScore = determineSquareCost(x, y + 1, 't');
		to = true;
		cout << "top square cost " << int(topSquareScore) << "\n";
	}
	else {
		cout << "top square doesn't exist\n";
	}

	if (isValidSquare(x, y - 1)) {//Bounds check
		bottomSquareScore = determineSquareCost(x, y - 1, 'b');
		bo = true;
		cout << "bottom square cost " << int(bottomSquareScore) << "\n";
	}
	else {
		cout << "bottom square doesn't exist\n";
	}

	if (isValidSquare(x - 1, y)) {//Bounds check
		leftSquareScore = determineSquareCost(x - 1, y, 'l');
		le = true;
		cout << "left square score " << int(leftSquareScore) << "\n";
	}
	else {
		cout << "left square doesn't exist\n";
	}

	if (isValidSquare(x + 1, y)) {//Bounds check
		rightSquareScore = determineSquareCost(x + 1, y, 'r');
		ri = true;
		cout << "right square cost " << int(rightSquareScore) << "\n";
	}
	else {
		cout << "right square doesn't exist\n";
	}



	if (
		((to && (int(topSquareScore) > int(thresholdScore))) || !to) &&
		((bo && (int(bottomSquareScore) > int(thresholdScore))) || !bo) &&
		((le && (int(leftSquareScore) > int(thresholdScore))) || !le) &&
		((ri && (int(rightSquareScore) > int(thresholdScore))) || !ri)
		) {//If all squares that exist score above threshold

		//if (int(topSquareScore) > int(thresholdScore) && int(bottomSquareScore) > int(thresholdScore)&& int(leftSquareScore) > int(thresholdScore) && int(rightSquareScore) > int(thresholdScore)) {//If all squares score above threshold

			//Trigger retreat
		printStuff(x, y);
		cout << "call retreat\n";
		strategy = -1;
		return retreat();
	}
	else {//At least one of the squares that exist scores <= threshold score


		//Check each of the squares whose score is less than the threshold score and only consider those that have not been visited
		bool topVisited = false;
		bool bottomVisited = false;
		bool leftVisited = false;
		bool rightVisited = false;

		//Check which ones have been visited
		if (visitedMap[x][y + 1] == 1) {
			topVisited = true;
		}

		if (visitedMap[x][y - 1] == 1) {
			bottomVisited = true;
		}

		if (visitedMap[x - 1][y] == 1) {
			leftVisited = true;
		}

		if (visitedMap[x + 1][y] == 1) {
			rightVisited = true;
		}

		//cout << "no retreat\n";
				//Implement a simple stack to push squares which are <= threshold 0 for top, 1 for bottom, 2 for left, 3 for right
		int stackSize = 0;
		int stackTop = -1;
		int safeStack[4];


		/*if (to && !topVisited && (int(topSquareScore) <= int(thresholdScore))) {//If top square exists and has not been visited and is <= thresholdScore
			stackTop++; stackSize++; safeStack[stackTop] = 0;
			bestScoreSoFar = int(topSquareScore);
		}*/

		if (to && !topVisited && (int(topSquareScore) == bestScoreSoFar)) {
			stackTop++; stackSize++; safeStack[stackTop] = 0;//Place it in stack
			//No need to update bestScoreSoFar
		}
		else if (to && !topVisited && (int(topSquareScore) < bestScoreSoFar)) {
			//Clear stack and only bottom square will be in stack
			stackTop = 0; stackSize = 1; safeStack[stackTop] = 0;
			bestScoreSoFar = int(bottomSquareScore);
		}

		if (bo && !bottomVisited && (int(bottomSquareScore) == bestScoreSoFar)) {
			stackTop++; stackSize++; safeStack[stackTop] = 1;//Place it in stack
			//No need to update bestScoreSoFar
		}
		else if (bo && !bottomVisited && (int(bottomSquareScore) < bestScoreSoFar)) {
			//Clear stack and only bottom square will be in stack
			stackTop = 0; stackSize = 1; safeStack[stackTop] = 1;
			bestScoreSoFar = int(bottomSquareScore);
		}

		if (le && !leftVisited && (int(leftSquareScore) == bestScoreSoFar)) {
			stackTop++; stackSize++; safeStack[stackTop] = 2;//Place it in stack
			//No need to update bestScoreSoFar
		}
		else if (le && !leftVisited && (int(leftSquareScore) < bestScoreSoFar)) {
			//Clear stack and only left square will be in stack
			stackTop = 0; stackSize = 1; safeStack[stackTop] = 2;
			bestScoreSoFar = int(leftSquareScore);
		}

		if (ri && !rightVisited && (int(rightSquareScore) == bestScoreSoFar)) {
			stackTop++; stackSize++; safeStack[stackTop] = 3;//Place it in stack
			//No need to update bestScoreSoFar
		}
		else if (ri && !rightVisited && (int(rightSquareScore) < bestScoreSoFar)) {
			//Clear stack and only right square will be in stack
			stackTop = 0; stackSize = 1; safeStack[stackTop] = 3;
			bestScoreSoFar = int(rightSquareScore);
		}


		//SafeStack will have at least one square at this point or maybe 0
		if (stackSize == 1) {
			chosenSquareIndex = safeStack[0];

			cout << "Only unvisited safe square is ";
			for (int i = 0; i < stackSize; i++) {
				if (safeStack[0] == 0) {
					cout << "top/n";
				}
				else if (safeStack[0] == 1) {
					cout << "bottom\n";
				}
				else if (safeStack[0] == 2) {
					cout << "left\n";
				}
				else if (safeStack[0] == 3) {
					cout << "right\n";
				}
			}

		}
		else if (stackSize > 1) {
			int rando = rand() % stackSize;//Generate some random number between 0 and stackSize - 1
			chosenSquareIndex = safeStack[rando];

			cout << "At least 2 unvisited safe squares:\n";
			for (int i = 0; i < stackSize; i++) {
				if (safeStack[i] == 0) {
					cout << "top, ";
				}
				else if (safeStack[i] == 1) {
					cout << "bottom, ";
				}
				else if (safeStack[i] == 2) {
					cout << "left, ";
				}
				else if (safeStack[i] == 3) {
					cout << "right\n";
				}
			}
			cout << "The one chosen was index " << safeStack[rando] << "\n";

		}
		else if (stackSize == 0) {
			printStuff(x, y);
			cout << "Retreating from within chooseAction() func\n";
			strategy = -1;
			return retreat();

		}
		else {
			cout << "Something weird happened. Satck must contain at least 0 element";
		}


		/*cout << "Unvisited safe squares are:\n";
		for (int i = 0; i < stackSize; i++) {
			if (safeStack[i] == 0) {
				cout << "top, ";
			}
			else if (safeStack[i] == 1) {
				cout << "bottom, ";
			}
			else if (safeStack[i] == 2) {
				cout << "left, ";
			}
			else if (safeStack[i] == 3) {
				cout << "right\n";
			}
		}*/

		cout << "no retreat\n";
		printStuff(x, y);
		//At this point, we have either the square with lowest score or a random square among those tied for lowest score
		strategy = 1;

		if (chosenSquareIndex == 0) {
			chosenCol = x;
			chosenRow = y + 1;
		}
		else if (chosenSquareIndex == 1) {
			chosenCol = x;
			chosenRow = y - 1;
		}
		else if (chosenSquareIndex == 2) {
			chosenCol = x - 1;
			chosenRow = y;
		}
		else if (chosenSquareIndex == 3) {
			chosenCol = x + 1;
			chosenRow = y;
		}


		//Update current row and col before moving to new square
		updateCurrentSquare(chosenCol, chosenRow);
		/*currentCol = chosenCol;
		currentRow = chosenRow;
		//Increment moves
		moves++;*/

		//Put the chosen square on stack since we just inferred it may be safe and we have already decided to go there anyways. Pop it if bump is detected(see bump)
		Cell c(chosenCol, chosenRow);//updates currentCol and currentRow to be the next square we are moving to and also increments move
		push(c);
		//Update KB accordingly
		KB[chosenCol][chosenRow].safe = true;
		//Update visitedMap accordingly
		visitedMap[chosenCol][chosenRow] = 1;


		cout << "\n\n\tChosen square is (col " << currentCol << ", row " << currentRow << ") with cost" << bestScoreSoFar << "\n";

		return determineTurnDirection(x, y, currentCol, currentRow);//Old x old y, new x, new y 
		//return determineTurnDirection(x, y, chosenCol, chosenRow);

	}


}


void MyAI::updateCurrentSquare(int c, int r) {
	currentCol = c;
	currentRow = r;
	//Increment moves
	//moves++;
}


Agent::Action MyAI::retreat() {
	cout << "implementing retreat within retreat() func from (" << currentCol << ", " << currentRow << ")\n";
	if (currentCol == 0 && currentRow == 0) {//If agent finds itself in (0,0)
		cout << "must climb\n";
		return CLIMB;
	}
	cout << "cannot climb :(\n";

	int formerX = currentCol;
	int formerY = currentRow;
	pop();//Pop current cell which will be at TOS

		//Next cell to go to next cell in retreat stack
		/*Cell c;
		c.col = peak().col;
		c.row = peak().row;*/

	int chosenX = peak().col;
	int chosenY = peak().row;

	cout << "retreating to (" << chosenX << ", " << chosenY << ")\n";
	//updates currentCol and currentRow to be the next square we are moving to and also increments move
	updateCurrentSquare(chosenX, chosenY);

	//Go to the chosenSquare
	return determineTurnDirection(formerX, formerY, chosenX, chosenY);
}


Agent::Action MyAI::determineTurnDirection(int oldX, int oldY, int newX, int newY) {//Determines if we make a turn or just move forward
	cout << "determining turn direction(oldX " << oldX << ", oldY " << oldY << ", newX " << newX << ", newY " << newY << "\n";
	//If only one action generated, return it, otherwise place the later actions in queue, earliest first, then excecute the earliest action(not forgeting to update direction if a turn)
	if (newY == oldY && newX == oldX + 1) {
		if (directionFacing == 'r') {
			moves++;
			return FORWARD;
		}
		else if (directionFacing == 'u') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
		else if (directionFacing == 'l') {//Could also be  L L F
			enqueue(TURN_RIGHT);
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
		else if (directionFacing == 'd') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_LEFT);
			return TURN_LEFT;
		}
	}
	else if (newY == oldY && newX == oldX - 1) {
		if (directionFacing == 'r') {//Could also be R R F
			enqueue(TURN_LEFT);
			enqueue(FORWARD);

			updateDirectionFacing(TURN_LEFT);
			return TURN_LEFT;
		}
		else if (directionFacing == 'u') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_LEFT);
			return TURN_LEFT;
		}
		else if (directionFacing == 'l') {
			moves++;
			return FORWARD;
		}
		else if (directionFacing == 'd') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
	}
	else if (newY == oldY - 1 && newX == oldX) {
		if (directionFacing == 'r') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
		else if (directionFacing == 'u') {//Could also be L L F
			enqueue(TURN_RIGHT);
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return(TURN_RIGHT);
		}
		else if (directionFacing == 'l') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_LEFT);
			return TURN_LEFT;
		}
		else if (directionFacing == 'd') {
			moves++;
			return FORWARD;
		}
	}
	else if (newY == oldY + 1 && newX == oldX) {
		if (directionFacing == 'r') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_LEFT);
			return TURN_LEFT;
		}
		else if (directionFacing == 'u') {
			moves++;
			return FORWARD;
		}
		else if (directionFacing == 'l') {
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
		else if (directionFacing == 'd') {//Could also be L L F
			enqueue(TURN_RIGHT);
			enqueue(FORWARD);

			updateDirectionFacing(TURN_RIGHT);
			return TURN_RIGHT;
		}
	}


}


void MyAI::printStuff(int x, int y) {

	cout << "Agent in square(" << x << "," << y << ")\n";

	cout << "current KB\n";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << "| (" << i << "," << j << ") ";
			if (KB[i][j].safe) {
				cout << "sa,";
			}
			else {
				cout << "---";
			}

			if (KB[i][j].breeze) {
				cout << "br,";
			}
			else {
				cout << "---";
			}

			if (KB[i][j].stench) {
				cout << "st,";
			}
			else {
				cout << "---";
			}
			cout << " |";
		}
		cout << "\n";
	}

	cout << "Retreat stack\n||";
	for (int i = 0; i <= top; i++) {
		cout << "(" << retreatStack[i].col << "," << retreatStack[i].row << ")|";
	}
	cout << "\n";

}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================