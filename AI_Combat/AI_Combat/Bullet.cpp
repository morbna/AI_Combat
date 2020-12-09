#include "Bullet.h"
#include "Actor.h"
#include "glut.h"
#include "const.h"
#include <math.h>

Bullet::Bullet(double x, double y, double dx, double dy, int team, bool mv, bool visible) :
	MovingEntity(x, y, dx, dy, team, SPEED * 10, mv), travelledDistance(0), hit(false), landed(false), visible(visible) {}


void Bullet::render(int maze[][MSZ])
{
	if (!visible)
		return;

	if(team==TEAM_A)
		glColor3d(0.2, 0.2, 0.8);
	else
		glColor3d(0.8, 0.2, 0.2);

	glBegin(GL_POLYGON);
	glVertex2d(x, y + 0.005);
	glVertex2d(x + 0.005, y);
	glVertex2d(x, y - 0.005);
	glVertex2d(x - 0.005, y);
	glEnd();
}

void Bullet::move(int maze[][MSZ])
{
	int row = getRow();
	int col = getCol();

	if (maze[row][col] == WALL) // end of flight
	{
		moving = false;
		landed = true;
	}
	else if (moving)
	{
		x += dx * speed;
		y += dy * speed;
		travelledDistance += sqrt(pow(dx * speed, 2) + pow(dy * speed, 2));
	}
}

void Bullet::generateDangerMap(double map[][MSZ])
{
	int row = getRow();
	int col = getCol();

	map[row][col] += DANGER_DELTA;
	if (map[row][col] > 1)
		map[row][col] = 1;
}

bool Bullet::hasLineOfSight(int maze[][MSZ], double tx, double ty)
{
	int row, col, tRow, tCol;

	while (true)
	{
		row = getRow();
		col = getCol();

		tRow = (int)(MSZ * (ty + 1) / 2);
		tCol = (int)(MSZ * (tx + 1) / 2);

		if (maze[row][col] == WALL) // hit WALL
			return false;

		if (row == tRow && col == tCol) // hit TARGET
			return true;

		x += dx * speed;
		y += dy * speed;
	}
	return true;
}

void Bullet::checkIfHit(std::vector<Actor*>& actors)
{
	if (landed)
		return;

	for (Actor* a : actors)
	{
		if (a->isHit(*this))
		{
			a->hit(*this);
			hit = true;
		}
	}
}

bool Bullet::hasHit() const
{
	return hit;
}

double Bullet::getTravelledDistance()
{
	return travelledDistance;
}
