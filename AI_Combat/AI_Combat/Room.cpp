#include "Room.h"
#include "const.h"
#include <math.h>

Room::Room()
{
	cx = MSZ / 2;
	cy = MSZ / 2;
	width = MSZ / 2;
	height = MSZ / 2;
}

Room::Room(int row, int col, int w, int h)
{
	cy = row;
	cx = col;
	width = w;
	height = h;
}

void Room::setCenterX(int col)
{
	cx = col;
}

void Room::setCenterY(int row)
{
	cy = row;
}

void Room::setWidth(int w)
{
	width = w;
}

void Room::setHeight(int h)
{
	height = h;
}

int Room::getCenterX()
{
	return cx;
}

int Room::getCenterY()
{
	return cy;
}

int Room::getWidth()
{
	return width;
}

int Room::getHeight()
{
	return height;
}

void Room::init(int m[MSZ][MSZ])
{
	for (int i = cy - height / 2; i <= cy + height / 2; i++)
		for (int j = cx - width / 2; j <= cx + width / 2; j++)
			m[i][j] = SPACE;
	
}

bool Room::overlap(int w, int h, int cx, int cy)
{
	int hdist, vdist; // distance between rooms center
	
	hdist = abs(cx - this->cx);
	vdist = abs(cy - this->cy);
	
	return (this->width / 2 + w / 2) >= hdist - 2 && (this->height / 2 + h / 2) >= vdist - 2;
}
