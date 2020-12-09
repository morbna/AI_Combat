#ifndef  __ROOM_H__
#define  __ROOM_H__
#include "const.h"

class Room
{
private:
	int cx, cy, width, height;
public:
	Room();
	Room(int row, int col, int w, int h);

	void setCenterX(int col);
	void setCenterY(int row);
	void setWidth(int w);
	void setHeight(int h);

	int getCenterX();
	int getCenterY();
	int getWidth();
	int getHeight();
	
	void init(int m[MSZ][MSZ]);
	bool overlap(int w, int h, int cx, int cy);
};

#endif /* __ROOM_H__ */
