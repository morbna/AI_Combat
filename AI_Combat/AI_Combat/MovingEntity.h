#ifndef  __MOVING_ENTITY_H__
#define  __MOVING_ENTITY_H__
#include "Entity.h"

class MovingEntity: public Entity
{
protected:
	double dx, dy;
	bool moving;
	int team;
	double speed;
public:
	MovingEntity(double x, double y, double dx, double dy, int team, double speed, bool mv) :
		Entity(x, y), dx(dx), dy(dy), speed(speed), moving(mv), team(team) {};
	bool isMoving() const { return moving; }
	int getTeam() { return team; }
	virtual void move(int maze[][MSZ]) = 0;
};

#endif /* __MOVING_ENTITY_H__ */
