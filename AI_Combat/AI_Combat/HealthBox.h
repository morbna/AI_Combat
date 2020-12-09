#ifndef  __HEALTH_BOX_H__
#define  __HEALTH_BOX_H__
#include "LootableBox.h"

class HealthBox: public LootableBox
{
public:
	HealthBox();
	HealthBox(double x, double y);
	void render(int maze[][MSZ]) override;
	void loot(Actor& a) override;
};

#endif /* __HEALTH_BOX_H__ */
