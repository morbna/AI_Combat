#ifndef  __BULLET_H__
#define  __BULLET_H__
#include "MovingEntity.h"
#include "const.h"
#include "Room.h"
#include <vector>

class Actor;

class Bullet: public MovingEntity
{
	double travelledDistance;
	bool hit, landed, visible;

public:
	Bullet(double x, double y, double dx, double dy, int team, bool mv = true, bool visible = true);
	void render(int maze[][MSZ]) override;
	void move(int maze[][MSZ]) override;
	void generateDangerMap(double map[][MSZ]);
	bool hasLineOfSight(int maze[][MSZ], double tx, double ty);
	void checkIfHit(std::vector<Actor*>& actors);
	bool hasHit() const;
	double getTravelledDistance();
};

#endif /* __BULLET_H__ */

