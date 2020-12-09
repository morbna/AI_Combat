#include "HealthBox.h"
#include "glut.h"

HealthBox::HealthBox() : LootableBox(0, 0)
{
}

HealthBox::HealthBox(double x, double y) : LootableBox(x, y)
{
}

void HealthBox::render(int maze[][MSZ])
{
	glColor3d(1, 0.5, 0.5);
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

void HealthBox::loot(Actor& a)
{
	a.restoreHealth();
}
