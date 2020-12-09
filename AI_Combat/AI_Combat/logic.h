#ifndef  __LOGIC_H__
#define  __LOGIC_H__
#include "const.h"
#include "Node.h"
#include "Room.h"
#include "LootableBox.h"
#include "Bullet.h"
#include <vector>
#include <queue>

using namespace std;

void drawMaze(int maze[][MSZ], double dangerMap[][MSZ], bool danger);

void generateDangerMap(int maze[][MSZ], double dangerMap[][MSZ], vector<Bullet>& bullets);

double distance(int x1, int y1, int x2, int y2);

void updatePQ(priority_queue < Node*, vector<Node*>, CompareNodes>& pq, Node* pn);

void restorePath(Node* pCurrent, int maze[][MSZ], int ignore, int fill);

void addNeighbour(int tx, int ty, Node* pCurrent, DIRECTION direction,
	priority_queue < Node*, vector<Node*>, CompareNodes>& pq,
	vector <Node*>& gray, vector <Node*>& black,
	int maze[][MSZ], HEURISTIC hr);

void aStar(int tx, int ty, int maze[][MSZ], Node* startNode, int target, int ignore, int fill, bool diag, HEURISTIC hr);

void digPath(int index1, int index2, int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS]);

void digPaths(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS]);

void placeLootableBox(int maze[][MSZ], Room& room, LootableBox* box);

void placeLootableBoxes(int maze[][MSZ], Room rooms[NUM_ROOMS], vector<LootableBox*>& boxes);

void placeBoxes(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS]);

void placeActors(int maze[][MSZ], Room rooms[NUM_ROOMS], std::vector<Actor*>& actors);

void setupMaze(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS]);

#endif /* __LOGIC_H__ */
