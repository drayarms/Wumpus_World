// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"


struct Cell {
	int col;
	int row;
	Cell();//Default constructor
	Cell(int c, int r);//Constructor
};


struct SquareKB {
	bool safe;//Squares that we visit are assumed safe, otherwise not
	bool breeze;
	bool stench;
	bool gold;
	SquareKB();//Default constructor
	SquareKB(bool sa, bool br, bool st, bool gl);//Explicit constructor
};



class MyAI : public Agent
{
public:
	MyAI(void);

	Action getAction
	(
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
	);

	// ======================================================================
	// YOUR CODE BEGINS
	//void updateMaps(float map[][7], int x, int y);
	bool isValidSquare(int x, int y);//Ensures square is within bounds [0-7][0-7]
	Action chooseAction(int x, int y);//Choose the best action given agent is in col x row y
	Action determineTurnDirection(int oldX, int oldY, int newX, int newY);//Determine which direction to turn base on what col x and row y agent has chosen
	void updateDirectionFacing(Action directionTurned);
	void updateCurrentSquare(int c, int r);
	float determineSquareCost(int x, int y, char squarePositionRelativeToAgent);
	Action retreat();
	void printStuff(int x, int y);

	//actionsQueue methods
	Action dequeue();
	void enqueue(Action act);
	bool actionsQueueEmpty();
	bool actionsQueueFull();


	/*struct Cell {
		int col;
		int row;
		Cell(int c, int r);//Constructor
	};*/


	//retreatStackMethods
	bool retreatStackEmpty();
	void push(Cell c);
	void pop();
	Cell peak();


	/*struct SquareKB {
		bool safe;//Squares that we visit are assumed safe, otherwise not
		bool breeze;
		bool stench;
		bool gold;
		SquareKB(bool sa, bool br, bool st, bool gl);//Explicit constructor
	};*/
	// ======================================================================
private:

	SquareKB KB[7][7];
	Cell retreatStack[49];
	Action actionsQueue[200];
	int front;
	int rear;
	int qSize;
	int sSize;
	int top;
	int moves;//Num moves made by agent
	int wumpusCost;
	int pitCost;
	bool arrowUsed;
	bool wumpusDead;
	bool gotTheGold;
	int worldDimensionX;
	int worldDimensionY;
	int strategy;//1 for gold search, -1 for retreat. Switch from 1 to -1 if gold is found or if there no safe square to move to. -1 to 1 by defualt, and if while retreating, some safe unexplored sqare is found
	//float topSquareScore;
	//float bottomSquareScore;
	//float leftSquareScore;
	//float rightSquareScore;
	float thresholdScore;

	const Action actions[6] =
	{
		TURN_LEFT,
		TURN_RIGHT,
		FORWARD,
		SHOOT,
		GRAB,
		CLIMB
	};

	// ======================================================================
	// YOUR CODE ENDS
	char directionFacing;
	int currentCol;//x val
	int currentRow;//y value
	int visitedMap[7][7];//7X7 in the max dimension of any possible world
	//float wumpusMap[7][7];
	//float pitMap[7][7];
	// ======================================================================
};

#endif#pragma once
