#include "Grenade.h"
#include "glut.h"
#include <math.h>

Grenade::Grenade(double x, double y, double dx, double dy, double tx, double ty, int team, bool mv) :
	MovingEntity(x, y, dx, dy, team, SPEED * 5, mv), targetX(tx), targetY(ty), exploded(false), ticks(0)
{
	if (x == tx && y == ty)
		moving = false;
}

void Grenade::tick(std::vector<Bullet>& bullets)
{
	ticks += 1;

	if (ticks > TICK_TIME)
		explode(bullets);
}

void Grenade::explode(std::vector<Bullet>& bullets)
{
	double alpha = 0, theta = 2 * PI / NUM_BULLETS;

	// create bullets
	for (int i = 0; i < NUM_BULLETS; i++, alpha += theta)
		bullets.push_back(Bullet(x, y, cos(alpha), sin(alpha), team));

	exploded = true;
}

void Grenade::move(int maze[][MSZ])
{

}

void Grenade::move(int maze[][MSZ], std::vector<Bullet>& bullets)
{
	int row = getRow();
	int col = getCol();

	int targetRow = (int)(MSZ * (targetY + 1) / 2);
	int targetCol = (int)(MSZ * (targetX + 1) / 2);

	if (maze[row][col] == WALL ) // end of flight
		moving = false;
	else if (row = targetRow && col == targetCol) // landed
		moving = false;
	else if (moving)
	{
		x += dx * speed;
		y += dy * speed;
	}

	if(!moving)
		tick(bullets);
}

void Grenade::render(int maze[][MSZ])
{
	if (team == TEAM_A)
		glColor3d(0.1, 0.1, 0.5);
	else
		glColor3d(0.5, 0.1, 0.1);

	glBegin(GL_POLYGON);
	glVertex2d(x, y + 0.01);
	glVertex2d(x + 0.01, y);
	glVertex2d(x, y - 0.01);
	glVertex2d(x - 0.01, y);
	glEnd();
}

bool Grenade::hasExploded() const
{
	return exploded;
}