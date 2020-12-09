#ifndef  __AMMO_BOX_H__
#define  __AMMO_BOX_H__
#include "LootableBox.h"

class AmmoBox: public LootableBox
{
public:
	AmmoBox();
	AmmoBox(double x, double y);
	void render(int maze[][MSZ]) override;
	void loot(Actor& a) override;
};

#endif /* __AMMO_BOX_H__ */
