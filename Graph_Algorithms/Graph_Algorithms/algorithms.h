#ifndef  __ALGO_H__
#define  __ALGO_H__

#include "const.h"
#include "Node.h"
#include <queue>
#include <vector>

bool computeG(Node* n1, Node* n2);
bool computeH(Node* n1, Node* n2);

// maze
extern vector<Node*> gray;
extern vector<Node*> grayReverse;

// best first search maze
extern priority_queue <Node*, vector<Node*>, decltype(&computeG)> pq_gray_bfs;

// A* maze
extern priority_queue <Node*, vector<Node*>, decltype(&computeH)> pq_gray_astar;

int computeManhattanDistance(Node* n, Node* ref);

void setupMaze(int maze[MSZ][MSZ]);

bool BFSIteration(int maze[MSZ][MSZ]);

bool DFSIteration(int maze[MSZ][MSZ]);

Node* scanDirection(int maze[MSZ][MSZ],vector<Node*>& _gray, int color, int other);

bool BFSBiDiIteration(int maze[MSZ][MSZ]);

bool BestFirstSearchIteration(int maze[MSZ][MSZ]);

bool AStarIteration(int maze[MSZ][MSZ]);

void drawPath(int maze[MSZ][MSZ], Node* pCurrent, const int FLAG = START);

#endif /* __ALGO_H__ */
