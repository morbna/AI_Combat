#include "logic.h"
#include "Grenade.h"
#include "AmmoBox.h"
#include "HealthBox.h"
#include <iostream>
#include "glut.h"

void drawMaze(int maze[][MSZ], double dangerMap[][MSZ], bool danger)
{
	int i, j;
	double dx, dy, x, y, s;

	dx = dy = SCALE; // 2.0 is the logical width and height of the window

	y = -1;
	for (i = 0; i < MSZ; i++, y += dy)
	{
		x = -1;
		for (j = 0; j < MSZ; j++, x += dx)
		{
			switch (maze[i][j]) {
			case WALL:
				glColor3d(0.3, 0.3, 0.3);
				break;
			case SPACE:
				s = 1;
				if (danger)
					s = 1 - dangerMap[i][j];
				glColor3d(s, s, s);
				break;
			case START:
				glColor3d(0.2, 0.4, 1);
				break;
			case TARGET:
				glColor3d(1, 0, 0);
				break;

			}// switch

			// draw one square in maze
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + dy);
			glVertex2d(x + dx, y + dy);
			glVertex2d(x + dx, y);
			glEnd();
		}
	}
}

void generateDangerMap(int maze[][MSZ], double dangerMap[][MSZ], vector<Bullet>& bullets)
{	
	bullets.reserve(MSZ * MSZ);

	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			double x = i / (double)MSZ * 2 - 1;
			double y = j / (double)MSZ * 2 - 1;

			Grenade p = Grenade(x, y, 0, 0, x, y, TEAM_A);
			p.explode(bullets);
		}
	}
	
	while (!bullets.empty())
	{
		for (Bullet& b : bullets)
		{
			b.move(maze);
			b.generateDangerMap(dangerMap);
		}
		// iterator to non-moving bullets
		auto bVec = remove_if(begin(bullets), end(bullets), [](const Bullet& bullet) {return !bullet.isMoving(); });

		// erase non-moving bullets
		bullets.erase(bVec, bullets.end());
	}

	bullets.shrink_to_fit();

	double max = 0;
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			if (dangerMap[i][j] > max)
				max = dangerMap[i][j];
}

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void updatePQ(priority_queue < Node*, vector<Node*>, CompareNodes>& pq, Node* pn)
{
	bool found = false;
	Node* ptmp;
	vector<Node*> tmp_v;

	while (!pq.empty() && !found)
	{
		ptmp = pq.top();
		pq.pop();
		if (ptmp->operator==(*pn))
		{
			found = true;
			pq.push(pn);
		}
		else
			tmp_v.push_back(ptmp);
	}

	while (!tmp_v.empty())
	{
		ptmp = tmp_v.back();
		tmp_v.pop_back();
		pq.push(ptmp);
	}
}

void restorePath(Node* pCurrent, int maze[][MSZ], int ignore, int fill)
{
	while (maze[pCurrent->getRow()][pCurrent->getCol()] != ignore)
	{
		maze[pCurrent->getRow()][pCurrent->getCol()] = fill;
		pCurrent = pCurrent->getParent();
		if (pCurrent == nullptr)
			break;
	}
	if (pCurrent)
		maze[pCurrent->getRow()][pCurrent->getCol()] = fill;
}

void addNeighbour(int tx, int ty, Node* pCurrent, DIRECTION direction,
	priority_queue < Node*, vector<Node*>, CompareNodes>& pq,
	vector <Node*>& gray, vector <Node*>& black,
	int maze[][MSZ], HEURISTIC hr)
{
	int i = 0, j = 0;
	double g = 0, h = 0;
	Node* pn;
	vector<Node*>::iterator it_gray, it_black;

	int row = pCurrent->getRow();
	int col = pCurrent->getCol();
	double pg = pCurrent->getG();

	switch (direction)
	{
	case DIRECTION::TOP:
		i = row + 1;
		j = col;
		break;
	case DIRECTION::BOTTOM:
		i = row - 1;
		j = col;
		break;
	case DIRECTION::LEFT:
		i = row;
		j = col - 1;
		break;
	case DIRECTION::RIGHT:
		i = row;
		j = col + 1;
		break;
	case DIRECTION::DIAG_TL:
		i = row + 1;
		j = col - 1;
		if (maze[i][col] == WALL || maze[row][j] == WALL)
			g = INVALID_COST;
		break;
	case DIRECTION::DIAG_BL:
		i = row - 1;
		j = col - 1;
		if (maze[i][col] == WALL || maze[row][j] == WALL)
			g = INVALID_COST;
		break;
	case DIRECTION::DIAG_TR:
		i = row + 1;
		j = col + 1;
		if (maze[i][col] == WALL || maze[row][j] == WALL)
			g = INVALID_COST;
		break;
	case DIRECTION::DIAG_BR:
		i = row + 1;
		j = col + 1;
		if (maze[i][col] == WALL || maze[row][j] == WALL)
			g = INVALID_COST;
		break;
	}

	h = distance(j, i, tx, ty);

	if (hr == HEURISTIC::ROOM)
	{
		if (maze[i][j] == WALL)
			g += (pg + WALL_COST);
		else
			g += (pg + SPACE_COST);
	}
	else
	{
		if (maze[i][j] == WALL)
			g += (pg + INVALID_COST);
		else
			g += (pg + SPACE_COST);
	}

	pn = new Node(i, j, pCurrent, g, h);

	// check the neighbour color
	it_black = find_if(black.begin(), black.end(), [pn](const Node* p) {return *pn == *p; });
	if (it_black != black.end()) // its black
	{
		delete pn;
		return;
	}
	else // its not black
	{
		it_gray = find_if(gray.begin(), gray.end(), [pn](const Node* p) {return *pn == *p; });
		if (it_gray == gray.end()) // not found -> it is white, add to pq and paint it gray
		{
			pq.push(pn);
			gray.push_back(pn);
		}
		else // its gray -> check if its F is better (lower) than its pq copy
		{
			if ((*it_gray)->getF() <= pn->getF()) // not better
			{
				delete pn;
				return;
			}
			else // its better
			{
				updatePQ(pq, pn);
				gray.erase(it_gray);
				gray.push_back(pn);
			}
		}
	}
}

void aStar(int tx, int ty, int maze[][MSZ],Node* startNode, int target, int ignore, int fill, bool diag, HEURISTIC hr)
{
	priority_queue < Node*, vector<Node*>, CompareNodes> pq;
	vector <Node*> gray, black;
	vector<Node*>::iterator it_gray, it_black;
	Node* pCurrent;

	// 3. add start to PQ and to Grays 
	pq.push(startNode);
	gray.push_back(startNode);

	// 4. while PQ is not empty
	while (!pq.empty())
	{
		// 4.1 pick the "BEST"( F = G+H ) Node from PQ
		pCurrent = pq.top();
		pq.pop();

		// 4.2 if it is TARGET, the best path has been found
		if (maze[pCurrent->getRow()][pCurrent->getCol()] == target)
		{
			// 4.2.1 Restore path
			restorePath(pCurrent, maze, ignore, fill);

			// delete mem
			for (Node* p : black)
				delete(p);
			for (Node* p : gray)
				delete(p);

			return;
		}
		else // pCurrent is not TARGET
		{
			// paint pCurrent  black
			black.push_back(pCurrent);

			// remove pCurrent from gray
			it_gray = find(gray.begin(), gray.end(), pCurrent);
			if (it_gray != gray.end()) // if it was found
				gray.erase(it_gray);

			// 4.3 Add neighbours to PQ and return to 4

			int row = pCurrent->getRow();
			int col = pCurrent->getCol();

			if (diag)
			{
				// check top left neighbour
				if (row + 1 < MSZ && col - 1 >= 0)
					addNeighbour(tx, ty, pCurrent, DIRECTION::DIAG_TL, pq, gray, black, maze, hr);

				// check bottom left neighbour
				if (row - 1 >= 0 && col - 1 >= 0)
					addNeighbour(tx, ty, pCurrent, DIRECTION::DIAG_BL, pq, gray, black, maze, hr);

				// check top right neighbour
				if (row + 1 < MSZ && col + 1 < MSZ)
					addNeighbour(tx, ty, pCurrent, DIRECTION::DIAG_TR, pq, gray, black, maze, hr);

				// check bottom right neighbour
				if (row - 1 >= 0 && col + 1 < MSZ)
					addNeighbour(tx, ty, pCurrent, DIRECTION::DIAG_BR, pq, gray, black, maze, hr);
			}
			// check top neighbour
			if (row + 1 < MSZ)
				addNeighbour(tx, ty, pCurrent, DIRECTION::TOP, pq, gray, black, maze, hr);

			// check bottom neighbour
			if (row - 1 >= 0)
				addNeighbour(tx, ty, pCurrent, DIRECTION::BOTTOM, pq, gray, black, maze, hr);

			// check left neighbour
			if (col - 1 >= 0)
				addNeighbour(tx, ty, pCurrent, DIRECTION::LEFT, pq, gray, black, maze, hr);

			// check right neighbour
			if (col + 1 < MSZ)
				addNeighbour(tx, ty, pCurrent, DIRECTION::RIGHT, pq, gray, black, maze, hr);
		}

	}
	if(DEBUG)
		std::cout << "ERROR: Priority queue is empty. The target was missed" << std::endl;
}

// build path between rooms
void digPath(int index1, int index2, int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS])
{
	Node* pn;
	int is, js, it, jt;

	is = rooms[index1].getCenterY();
	js = rooms[index1].getCenterX();
	it = rooms[index2].getCenterY();
	jt = rooms[index2].getCenterX();

	// 1. set center rooms[index1] as START
	maze[is][js] = START;

	// 2. set center rooms[index2] as TARGET
	maze[it][jt] = TARGET;

	pn = new Node(is, js, nullptr, 0, distance(js, is, jt, it));

	// start A*
	aStar(jt, it, maze, pn, TARGET, START, SPACE, false, HEURISTIC::ROOM);
}

void digPaths(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS])
{
	for (int i = 0; i < NUM_ROOMS - 1; i++)
		for (int j = i + 1; j < NUM_ROOMS; j++)
			digPath(i, j, maze, dangerMap, rooms);
}

void placeLootableBox(int maze[][MSZ], Room& room, LootableBox* box)
{
	int rcx = room.getCenterX();
	int rcy = room.getCenterY();
	int rcw = room.getWidth();
	int rch = room.getHeight();

	bool overlap = true;
	int boxY, boxX;

	while (overlap)
	{
		boxY = 1 + rcy - rch / 2 + rand() % (rch - 1);
		boxX = 1 + rcx - rcw / 2 + rand() % (rcw - 1);

		// check if box overlap
		overlap = false;

		if (maze[boxY][boxX] == WALL)
			overlap = true;
		if (maze[boxY+1][boxX] == WALL)
			overlap = true;
		if (maze[boxY-1][boxX] == WALL)
			overlap = true;
		if (maze[boxY][boxX+1] == WALL)
			overlap = true;
		if (maze[boxY][boxX-1] == WALL)
			overlap = true;

		if (maze[boxY - 1][boxX - 1] == WALL)
			overlap = true;
		if (maze[boxY + 1][boxX - 1] == WALL)
			overlap = true;
		if (maze[boxY - 1][boxX + 1] == WALL)
			overlap = true;
		if (maze[boxY + 1][boxX + 1] == WALL)
			overlap = true;
	}
	double x = boxX / (double)MSZ * 2 - 1;
	double y = boxY / (double)MSZ * 2 - 1;

	x = (int)(MSZ * (x + 1) / 2) * SCALE - 1 + ESCALE; // tile aligned
	y = (int)(MSZ * (y + 1) / 2) * SCALE - 1 + ESCALE; // tile aligned

	// set box
	box->setX(x);
	box->setY(y);
}

void placeLootableBoxes(int maze[][MSZ], Room rooms[NUM_ROOMS], vector<LootableBox*>& boxes)
{
	// place up to 2 boxes per type

	if (NUM_ROOMS > 0)
	{
		AmmoBox* a = new AmmoBox();
		placeLootableBox(maze, rooms[0], a);
		boxes.push_back(a);
	}
	if (NUM_ROOMS > 1)
	{
		HealthBox* h = new HealthBox();
		placeLootableBox(maze, rooms[1], h);
		boxes.push_back(h);
	}
	if (NUM_ROOMS > 2)
	{
		AmmoBox* a = new AmmoBox();
		placeLootableBox(maze, rooms[2], a);
		boxes.push_back(a);
	}
	if (NUM_ROOMS > 3)
	{
		HealthBox* h = new HealthBox();
		placeLootableBox(maze, rooms[3], h);
		boxes.push_back(h);
	}
}

void placeBoxes(int maze[][MSZ],double dangerMap[][MSZ], Room rooms[NUM_ROOMS])
{
	for (int i = 0; i < NUM_ROOMS ; i++)
	{
		Room r = rooms[i];
		int rcx = r.getCenterX();
		int rcy = r.getCenterY();
		int rcw = r.getWidth();
		int rch = r.getHeight();

		// place 1-3 boxes per room
		for (int j = 1 + rand() % 3; j > 0; j--) 
		{
			bool overlap = false;

			// generate box size 2-4
			int boxWidth = 2 + rand() % 3;
			int boxHeight = 2 + rand() % 3;

			int boxY = 1 + rcy - rch / 2 + rand() % (rch - boxHeight - 2);
			int boxX = 1 + rcx - rcw / 2 + rand() % (rcw - boxWidth - 2);

			// check if box overlap
			for (int k = boxWidth - 1; k >= 0; k--)
				for (int u = boxHeight - 1; u >= 0; u--)
					if (maze[boxY + k][boxX + u] == WALL)
						overlap = true;

			if (overlap)
			{
				j++;
				continue;
			}

			// place box
			for (int k = boxWidth - 1; k >= 0; k--)
				for (int u = boxHeight - 1; u >= 0; u--)
				{
					maze[boxY + k][boxX + u] = WALL;
					dangerMap[boxY + k][boxX + u] = 1;
				}
		}
	}
}

void placeActors(int maze[][MSZ], Room rooms[NUM_ROOMS], std::vector<Actor*>& actors)
{
	// choose 2 random rooms
	int r2, r1 = rand() % NUM_ROOMS;
	do { 
		r2 = rand() % NUM_ROOMS;
	} while (r2 == r1);

	int arr[2] = { r1, r2 };
	int team[2] = { TEAM_A, TEAM_B };

	for (int u = 0; u < 2; u++)
	{
		for (int k = 0; k < TEAM_SIZE; k++)
		{
			int i = rooms[arr[u]].getCenterX() + 1 + rand() % (rooms[arr[u]].getWidth() - 2) - rooms[arr[u]].getWidth() / 2;
			int j = rooms[arr[u]].getCenterY() + 1 + rand() % (rooms[arr[u]].getHeight() - 2) - rooms[arr[u]].getHeight() / 2;

			if (maze[j][i] == WALL)
			{
				--k;
				continue;
			}

			double x = i * SCALE - 1 + ESCALE;
			double y = j * SCALE - 1 + ESCALE;

			Actor* a = new Actor(x, y, team[u], maze);
			actors.push_back(a);
		}
	}
}

void setupMaze(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS])
{
	int i, j;
	int countRoom;
	int w, h, cx, cy;
	// set borders

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j] = WALL;

	for (countRoom = 0; countRoom < NUM_ROOMS; countRoom++)
	{
		bool hasOverlap;

		do {
			// pick room parameters
			w = 8 + rand() % (MSZ / 5);
			h = 8 + rand() % (MSZ / 5);
			cx = 1 + w / 2 + rand() % (MSZ - w - 2);
			cy = 1 + h / 2 + rand() % (MSZ - h - 2);

			// check room placement overlap
			hasOverlap = false;
			for (int k = 0; k < countRoom && !hasOverlap; k++)
				if (rooms[k].overlap(w, h, cx, cy))
					hasOverlap = true;

		} while (hasOverlap);

		// setup room
		rooms[countRoom].setWidth(w);
		rooms[countRoom].setHeight(h);
		rooms[countRoom].setCenterX(cx);
		rooms[countRoom].setCenterY(cy);
		rooms[countRoom].init(maze);
	}

	// build inter-room paths
	digPaths(maze, dangerMap, rooms);
}