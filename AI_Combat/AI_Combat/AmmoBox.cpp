#include "AmmoBox.h"
#include "glut.h"

AmmoBox::AmmoBox() :LootableBox(0, 0)
{
}

AmmoBox::AmmoBox(double x, double y) : LootableBox(x, y)
{
}

void AmmoBox::render(int maze[][MSZ])
{
	glColor3d(0.5, 0.5, 1);
	glBegin(GL_POLYGON);
	glVertex2d(x + 0.015, y + 0.01);
	glVertex2d(x + 0.015, y - 0.01);
	glVertex2d(x - 0.015, y - 0.01);
	glVertex2d(x - 0.015, y + 0.01);
	glEnd();

	glColor3d(0.1, 0.1, 0.1);
	glBegin(GL_POLYGON);
	glVertex2d(x + 0.005, y + 0.0025);
	glVertex2d(x + 0.005, y - 0.0025);
	glVertex2d(x - 0.005, y - 0.0025);
	glVertex2d(x - 0.005, y + 0.0025);
	glEnd();
}

void AmmoBox::loot(Actor& a)
{
	a.refillBullets();
	a.refillGrenades();
}
