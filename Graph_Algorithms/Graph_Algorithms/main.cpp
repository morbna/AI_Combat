#include "glut.h"
#include "const.h"
#include "Node.h"
#include "algorithms.h"
#include <time.h>
#include <iostream>

using namespace std;

int maze[MSZ][MSZ];

// maze
vector<Node*> gray;
vector<Node*> grayReverse;

// best first search maze
priority_queue <Node*, vector<Node*>, decltype(&computeG)> pq_gray_bfs(computeG);

// A* maze
priority_queue <Node*, vector<Node*>, decltype(&computeH)> pq_gray_astar(computeH);

bool START_BFS = false;
bool START_DFS = false;
bool START_BI_DIR_BFS = false;

bool START_BEST_FIRST_SEARCH = false;
bool START_A_STAR = false;

void init()
{
	// RED, GREEN, BLUE
	glClearColor(0.8, 0.8, 0.8, 0);// color of window background

	srand(time(0));
	// define main axes
	glOrtho(-1, 1, -1, 1, -1, 1); 

	setupMaze(maze);
}

void drawMaze() 
{
	int i, j;
	double dx, dy, x,y;

	dx = dy = 2.0 / MSZ; // 2.0 is the logical width and height of the window

	y = -1;
	for (i = 0; i < MSZ; i++,y+=dy)
	{
		x = -1;
		for (j = 0; j < MSZ; j++,x+=dx)
		{
			switch (maze[i][j]) {
			case WALL:
				glColor3d(0.2, 0.2, 0.2);
				break;
			case SPACE:
				glColor3d(1, 1, 1);
				break;
			case START:
				glColor3d(0.2, 0.4, 1);
				break;
			case TARGET:
				glColor3d(1, 0, 0);
				break;
			case BLACK_SPACE:
				glColor3d(1, 1, 0.5);
				break;
			case GRAY:
			case GRAYREV:
				glColor3d(0, 0.8, 0);
				break;
			case PATH:
				glColor3d(1, 0.5, 1);
				break;

			}// switch
			// draw one square in maze
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y+dy);
			glVertex2d(x+dx, y+dy);
			glVertex2d(x+dx, y);
			glEnd();
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	drawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (START_BFS) // bfs
		START_BFS = BFSIteration(maze);
	else if (START_DFS) // dfs
		START_DFS = DFSIteration(maze);
	else if (START_BI_DIR_BFS) // bidirectional bfs
		START_BI_DIR_BFS = BFSBiDiIteration(maze);
	else if (START_BEST_FIRST_SEARCH) // best first search
	{
		START_BEST_FIRST_SEARCH = BestFirstSearchIteration(maze);
		Sleep(5);
	}
	else if (START_A_STAR) // A*
	{
		START_A_STAR = AStarIteration(maze);
		Sleep(5);
	}

	glutPostRedisplay(); // call to display again
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // BFS
		START_BFS = true;
		break;
	case 2: // DFS
		START_DFS = true;
		break;
	case 3: // Bidirectional BFS
		START_BI_DIR_BFS = true;
		break;
	case 4: // Bidirectional BFS
		START_BEST_FIRST_SEARCH = true;
		break;
	case 5: // Bidirectional BFS
		START_A_STAR= true;
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display); // refresh window function
	glutIdleFunc(idle); // kind of timer function

	glutCreateMenu(menu); // create menu
	glutAddMenuEntry("BFS", 1);
	glutAddMenuEntry("DFS", 2);
	glutAddMenuEntry("Bidirectional BFS ", 3);
	glutAddMenuEntry("Best First Search ", 4);
	glutAddMenuEntry("A-Star", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON); // attach right click to menu

	init();

	glutMainLoop();
	return 0;
}