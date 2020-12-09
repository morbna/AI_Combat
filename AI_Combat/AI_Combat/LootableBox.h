#ifndef  __LOOTABLE_BOX_H__
#define  __LOOTABLE_BOX_H__
#include "Entity.h"
#include "Actor.h"

class LootableBox : public Entity
{
public:
	LootableBox(double x, double y) :
		Entity(x, y) {}

	virtual void loot(Actor& a) = 0;
};

#endif /* __LOOTABLE_BOX_H__ */
