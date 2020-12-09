#ifndef  __ACTOR_H__
#define  __ACTOR_H__
#include "MovingEntity.h"
#include "const.h"
#include <vector>

class Bullet;
class Grenade;
class Room;
class LootableBox;

class Actor: public MovingEntity
{
public:
	enum class eState { 
		IDLE, 
		HIDE,
		LOOT,
		DEAD,
		MOVE_TO_HIDING,
		MOVE_TO_ENEMY,
		MOVE_TO_BOX
		};

private:
	double hp, maxHp;
	double armor;
	double hpThreshold;
	int bulletAmmo, maxBulletAmmo;
	int grenadeAmmo, maxGrenadeAmmo;
	int lastShotTime; // shoot cd
	int lastThrowTime; // grenade cd
	int lastSeekTime; // enemy pathing cd
	int lastHideTime; // room reposition cd
	eState state;	  // actor state
	int mazeCopy[MSZ][MSZ]; // pathfinding maze copy

public:
	Actor(double x, double y, int team, int maze[][MSZ],
		double maxHp = BASE_HP,
		double armor = BASE_ARMOR,
		double hpThreshold = BASE_HP_THRESHOLD,
		int maxBulletAmmo = BASE_BULLET_AMMO,
		int maxGrenadeAmmo = BASE_GRENADE_AMMO);

	void render(int maze[][MSZ]) override;
	void move(int maze[][MSZ]) override;

	void shoot(double dx, double dy, std::vector<Bullet>& bullets);
	void throwGrenade(double dx, double dy, double tx, double ty, std::vector<Grenade>& grenades);

	void act(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS],
		std::vector<Actor*>& actors, std::vector<LootableBox*>& boxes, std::vector<Bullet>& bullets, std::vector<Grenade>& grenades);
	void kill();
	void clearPathing();
	void findPath(int row, int col, HEURISTIC hr);
	void hide(double dangerMap[][MSZ], Room rooms[NUM_ROOMS]);

	bool isAlive();
	void hit(Bullet& b);
	bool isHit(Bullet& b);
	int getCurrentRoom(Room rooms[NUM_ROOMS]);
	double getAngle(double x, double y);
	void orient();

	void refillBullets();
	void refillGrenades();
	void restoreHealth();
};

#endif /* __ACTOR_H__ */
