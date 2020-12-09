#ifndef  __GRENADE_H__
#define  __GRENADE_H__
#include "MovingEntity.h"
#include "const.h"
#include "Bullet.h"
#include <vector>

class Grenade : public MovingEntity
{
private:
	double targetX, targetY;
	bool exploded;
	int ticks;

	void move(int maze[][MSZ]) override;
	void tick(std::vector<Bullet>& bullets);
public:
	Grenade(double x, double y, double dx, double dy, double tx, double ty, int team, bool mv = true);
	void explode(std::vector<Bullet>& bullets);
	void render(int maze[][MSZ]) override;
	void move(int maze[][MSZ], std::vector<Bullet>& bullets);
	bool hasExploded() const;
};

#endif /* __GRENADE_H__ */


