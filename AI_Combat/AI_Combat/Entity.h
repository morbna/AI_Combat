#ifndef  __ENTITY_H__
#define  __ENTITY_H__
#include "const.h"
#include <cmath>

class Entity
{
protected:
	double x, y;
public:
	Entity(double x, double y) :
		x(x), y(y) {};

	double getX() { return x; }
	double getY() { return y; }
	int getRow() 
	{ 
		return (int)(MSZ * (y + 1) / 2); 
	}
	int getCol() 
	{ 
		return (int)(MSZ * (x + 1) / 2); 
	}
	double getCenterY()
	{
		return getRow() * SCALE - 1 + ESCALE;
	}
	double getCenterX()
	{
		return getCol() * SCALE - 1 + ESCALE;
	}
	void setX(double x) { this->x = x; }
	void setY(double y) { this->y = y; }

	virtual void render(int maze[][MSZ]) = 0;
	double getDist(double tx, double ty) { return std::abs(x - tx) + std::abs(y - ty); }
};

#endif /* __ENTITY_H__ */
