#include "algorithms.h"
#include <iostream>

bool BI_BFS = false;

// manhattan distance to reference node
int computeManhattanDistance(Node* n, Node* ref)
{
	// reference node
	int rx = ref->getRow();
	int ry = ref->getCol();

	int x1 = n->getRow();
	int y1 = n->getCol();

	int dist = abs(x1 - rx) + abs(y1 - ry);

	return dist;
}

// G - manhattan distance to target
bool computeG(Node* n1, Node* n2)
{
	// dist to target node
	int dist1 = computeManhattanDistance(n1, grayReverse[0]);
	int dist2 = computeManhattanDistance(n2, grayReverse[0]);

	return dist1 > dist2;
}

// H - manhattan distance to start + target 
bool computeH(Node* n1, Node* n2)
{
	// dist to target node
	int dist1_t = computeManhattanDistance(n1, grayReverse[0]);
	int dist2_t = computeManhattanDistance(n2, grayReverse[0]);

	// dist to start node
	int dist1_s = computeManhattanDistance(n1, gray[0]);
	int dist2_s = computeManhattanDistance(n2, gray[0]);

	int dist1 = dist1_t + dist1_s;
	int dist2 = dist2_t + dist2_s;

	return dist1 > dist2;
}

void setupMaze(int maze[MSZ][MSZ])
{

	int i, j, r1, r2;
	// set borders

	for (i = 0; i < MSZ; i++)
	{
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
	{
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // mostly SPACEs (90%)
			{
				if (rand() % 10 > 0)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
			else // mostly WALLs (70%)
			{
				if (rand() % 10 > 2)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}
	}
	// set start point
	maze[MSZ / 2][MSZ / 2] = START;
	// set target point
	r1 = rand() % MSZ;
	r2 = rand() % MSZ;
	maze[r1][r2] = TARGET;

	// insert to gray the START node
	Node* pn = new Node(MSZ / 2, MSZ / 2, nullptr);
	gray.push_back(pn);

	// insert to grayReverse the TARGET node
	Node* pn2 = new Node(r1, r2, nullptr);
	grayReverse.push_back(pn2);

	// insert START to pq for informed searches
	pq_gray_bfs.push(pn);
	pq_gray_astar.push(pn);
}

bool BFSIteration(int maze[MSZ][MSZ])
{
	Node* pCurrent, * next;
	int cx, cy;

	if (gray.empty()) // no solution
	{
		cout << "there is no solution\n";
		return false;
	}
	else
	{
		// 1. dequeue next
		pCurrent = gray[0];
		gray.erase(gray.begin());

		// get current coords
		cx = pCurrent->getRow();
		cy = pCurrent->getCol();

		// 2. paint pCurrent black
		if (maze[cx][cy] != START)
			maze[cx][cy] = BLACK_SPACE;

		// 3. enqueue pCurrent's white neighbours
		// check all directions for possible paths

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (abs(i) != abs(j))
				{
					if (maze[cx + i][cy + j] == SPACE)
					{
						maze[cx + i][cy + j] = GRAY;
						next = new Node(cx + i, cy + j, pCurrent);
						gray.push_back(next);
					}
					else if (maze[cx + i][cy + j] == TARGET)
					{
						cout << "target found!\n";
						drawPath(maze, pCurrent);
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool DFSIteration(int maze[MSZ][MSZ])
{
	Node* pCurrent, * next;
	int cx, cy;

	if (gray.empty()) // no solution
	{
		cout << "there is no solution\n";
		return false;
	}
	else
	{
		// 1. last in
		pCurrent = gray[gray.size() - 1];

		// get current coords
		cx = pCurrent->getRow();
		cy = pCurrent->getCol();

		// 2. paint pCurrent black
		if (maze[cx][cy] != START)
			maze[cx][cy] = BLACK_SPACE;

		gray.pop_back();

		// add WHITE neighbours to gray

		vector<vector<int>> options{ {-1,0},{1,0},{0,-1},{0,1} };
		int directions[4] = { -1,-1,-1,-1 };
		int place, d, i, j;

		for (d = 0; d < 4; d++)
		{
			do
			{
				place = rand() % 4;
			} while (directions[place] != -1);
			directions[place] = d;
		}

		for (d = 0; d < 4; d++)
		{
			// RANDOM direction
			i = options[directions[d]][0];
			j = options[directions[d]][1];

			if (maze[cx + i][cy + j] == SPACE)
			{
				maze[cx + i][cy + j] = GRAY;
				next = new Node(cx + i, cy + j, pCurrent);
				gray.push_back(next);
			}
			else if (maze[cx + i][cy + j] == TARGET)
			{
				cout << "target found!\n";
				drawPath(maze, pCurrent);
				return false;
			}
		}
	}
	return true;
}

Node* scanDirection(int maze[MSZ][MSZ],vector<Node*>& _gray, int color, int other)
{
	Node* pCurrent, * next;
	int cx, cy;

	if (_gray.empty()) // no solution
	{
		cout << "there is no solution\n";
		BI_BFS = false;
	}
	else
	{
		// 1. dequeue next
		pCurrent = _gray[0];

		// get current coords
		cx = pCurrent->getRow();
		cy = pCurrent->getCol();

		// collision
		if (maze[cx][cy] == MERGE)
			return pCurrent;

		_gray.erase(_gray.begin());

		// 2. paint pCurrent black
		if (maze[cx][cy] != START && maze[cx][cy] != TARGET)
			maze[cx][cy] = BLACK_SPACE;

		// 3. enqueue pCurrent's white neighbours
		// check all directions for possible paths
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (abs(i) != abs(j))
				{
					if (maze[cx + i][cy + j] == SPACE)
					{
						maze[cx + i][cy + j] = color;
						next = new Node(cx + i, cy + j, pCurrent);
						_gray.push_back(next);
					}
					else if (maze[cx + i][cy + j] == other)
					{
						maze[cx + i][cy + j] = MERGE;
						next = new Node(cx + i, cy + j, pCurrent);
						_gray.push_back(next);
						return next;
					}
				}
			}
		}
	}
	return nullptr;
}

// Bidirection BFS
bool BFSBiDiIteration(int maze[MSZ][MSZ])
{
	BI_BFS = true;

	// forward scan
	Node* p = scanDirection(maze, gray, GRAY, GRAYREV);
	// backward scan
	Node* pr = scanDirection(maze, grayReverse, GRAYREV, GRAY);

	if (!BI_BFS)
		return false;

	if (p != nullptr && pr == nullptr)
	{
		cout << "Forward scan hit!\n";
		while (pr == nullptr)
			pr = scanDirection(maze, grayReverse, GRAYREV, MERGE);
	}
	else if (p == nullptr && pr != nullptr)
	{
		cout << "Backward scan hit!\n";
		while (p == nullptr)
			p = scanDirection(maze, gray, GRAY, MERGE);
	}

	if (p && pr)
	{
		cout << "Path found!\n";
		drawPath(maze, p, START);
		drawPath(maze, pr, TARGET);

		return false;
	}
	return true;
}

bool BestFirstSearchIteration(int maze[MSZ][MSZ])
{
	Node* pCurrent, * next;
	int cx, cy;

	if (pq_gray_bfs.empty()) // no solution
	{
		cout << "there is no solution\n";
		return false;
	}
	else
	{
		// 1. dequeue next
		pCurrent = pq_gray_bfs.top();
		pq_gray_bfs.pop();

		// get current coords
		cx = pCurrent->getRow();
		cy = pCurrent->getCol();

		// 2. paint pCurrent black
		if (maze[cx][cy] != START)
			maze[cx][cy] = BLACK_SPACE;

		// 3. enqueue pCurrent's white neighbours
		// check all directions for possible paths

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (abs(i) != abs(j))
				{
					if (maze[cx + i][cy + j] == SPACE)
					{
						maze[cx + i][cy + j] = GRAY;
						next = new Node(cx + i, cy + j, pCurrent);
						pq_gray_bfs.push(next);
					}
					else if (maze[cx + i][cy + j] == TARGET)
					{
						cout << "target found!\n";
						drawPath(maze, pCurrent);
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool AStarIteration(int maze[MSZ][MSZ])
{
	Node* pCurrent, * next;
	int cx, cy;

	if (pq_gray_astar.empty()) // no solution
	{
		cout << "there is no solution\n";
		return false;
	}
	else
	{
		// 1. dequeue next
		pCurrent = pq_gray_astar.top();
		pq_gray_astar.pop();

		// get current coords
		cx = pCurrent->getRow();
		cy = pCurrent->getCol();

		// 2. paint pCurrent black
		if (maze[cx][cy] != START)
			maze[cx][cy] = BLACK_SPACE;

		// 3. enqueue pCurrent's white neighbours
		// check all directions for possible paths

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (abs(i) != abs(j))
				{
					if (maze[cx + i][cy + j] == SPACE)
					{
						maze[cx + i][cy + j] = GRAY;
						next = new Node(cx + i, cy + j, pCurrent);
						pq_gray_astar.push(next);
					}
					else if (maze[cx + i][cy + j] == TARGET)
					{
						cout << "target found!\n";
						drawPath(maze, pCurrent);
						return false;
					}
				}
			}
		}
	}
	return true;
}

void drawPath(int maze[MSZ][MSZ],Node* pCurrent, const int FLAG)
{
	int len = 0;
	while (maze[pCurrent->getRow()][pCurrent->getCol()] != FLAG)
	{
		len++;
		maze[pCurrent->getRow()][pCurrent->getCol()] = PATH;
		pCurrent = pCurrent->getParent();
	}
	cout << "Path Length: " << len << endl;
}
