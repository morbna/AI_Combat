#include "glut.h"
#include "const.h"
#include "logic.h"
#include <time.h>
#include <math.h>
#include <vector>
#include "Bullet.h"
#include "Grenade.h"
#include "Room.h"
#include "Actor.h"
#include "LootableBox.h"

int maze[MSZ][MSZ] = { 0 };			// maze tiles
double dangerMap[MSZ][MSZ] = { 0 };	// tile safety map

Room rooms[NUM_ROOMS];

vector<Bullet> bullets;
vector<Grenade> grenades;

vector<Actor*> actors;
vector<LootableBox*> boxes;

bool MENU_BULLET = false;
bool MENU_GRENADE = false;
bool MENU_ACTOR = false;
bool MENU_DANGER = false;
bool MENU_TEAM = false;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	// draw maze tiles
	drawMaze(maze, dangerMap, MENU_DANGER);

	// draw entities
	for (Bullet& b : bullets)
		b.render(maze);

	for (Grenade& g : grenades)
		g.render(maze);

	for (LootableBox* b : boxes)
		b->render(maze);

	for (Actor* a : actors)
		a->render(maze);

	glutSwapBuffers(); // show all
}

void idle()
{
	for (Grenade& g : grenades)
		g.move(maze, bullets);

	for (Bullet& b : bullets)
	{
		b.move(maze);
		b.checkIfHit(actors);
	}

	for (Actor* a : actors)
		a->act(maze, dangerMap, rooms, actors, boxes, bullets, grenades);

	// erase exploded grenades
	auto gVec = remove_if(begin(grenades), end(grenades), [](const Grenade& grenade) {return grenade.hasExploded(); });
	grenades.erase(gVec, grenades.end());

	// erase hit bullets
	auto bVec = remove_if(begin(bullets), end(bullets), [](const Bullet& bullet) {return bullet.hasHit(); });
	bullets.erase(bVec, bullets.end());

	glutPostRedisplay(); // call to display again
}

void init()
{
	srand((int)time(NULL));

	// color of window background
	glClearColor(0, 0, 0, 0); 

	// define main axes
	glOrtho(-1, 1, -1, 1, -1, 1);

	// build maze and rooms
	setupMaze(maze, dangerMap, rooms);

	// place some boxes
	placeBoxes(maze, dangerMap, rooms);

	// place ammo/health boxes
	placeLootableBoxes(maze, rooms, boxes);

	// generate danger map
	generateDangerMap(maze, dangerMap, bullets);
	
	// place actors
	placeActors(maze, rooms, actors);
}

void destroy()
{
	for (LootableBox* b : boxes)
		delete b;
	for (Actor* a : actors)
		delete a;
}

void menu(int choice)
{
	MENU_BULLET = MENU_GRENADE = MENU_ACTOR = false;

	if (choice == 1)
		MENU_BULLET = true;
	else if (choice == 2)
		MENU_GRENADE = true;
	else if (choice == 3)
		MENU_ACTOR = true;
	else if (choice == 4)
		MENU_DANGER = !MENU_DANGER;
	else if (choice == 5)
		MENU_TEAM = !MENU_TEAM;
}

// x, y are in pixels
void mouse(int button, int state, int x, int y)
{
	double bx, by, dx, dy, alpha;;
	int team = MENU_TEAM ? TEAM_A : TEAM_B;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		bx = 2.0 * x / W - 1; // [-1,1]
		by = 2.0 * (H - y * 1.0) / H - 1; // [-1,1]

		bx = (int)(MSZ * (bx + 1) / 2) * SCALE - 1 + ESCALE; // tile aligned
		by = (int)(MSZ * (by + 1) / 2) * SCALE - 1 + ESCALE; // tile aligned

		alpha = (rand() % 360) * PI / 180; // deg -> rad
		dx = cos(alpha);
		dy = sin(alpha);

		if(MENU_BULLET)
			bullets.push_back(Bullet(bx, by, dx, dy, team));
		if (MENU_GRENADE)
			grenades.push_back(Grenade(bx, by, dx, dy, bx + dx * SPEED * 1000, by + dy * SPEED * 1000, team));
		if (MENU_ACTOR)
		{
			Actor* a = new Actor(bx, by, team, maze);
			actors.push_back(a);
		}
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 0);
	glutCreateWindow("Game");

	glutDisplayFunc(display); // refresh window function
	glutIdleFunc(idle); // kind of timer function

	glutCreateMenu(menu);
	glutAddMenuEntry("Shoot Bullet", 1);
	glutAddMenuEntry("Throw Grenade", 2);
	glutAddMenuEntry("Spawn Actor", 3);
	glutAddMenuEntry("Toggle Safety Map", 4);
	glutAddMenuEntry("Toggle Team", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMouseFunc(mouse);
	init();

	glutMainLoop();

	return 0;
}