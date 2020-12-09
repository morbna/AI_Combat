#ifndef  __CONST_H__
#define  __CONST_H__

const double PI = 3.14;

// console debug msgs
const bool DEBUG = false;

// window
const int W = 800; // window width
const int H = 800; // window height

// maze tiles
const int MSZ = 100;
const int WALL = 1;
const int SPACE = 2;

// AI
const int START = 3;
const int TARGET = 4;
const int PATH = 5;

// room
const int NUM_ROOMS = 10;

// pathing 
const double SPACE_COST = 0.1;
const double WALL_COST = 2;
const double INVALID_COST = WALL_COST * 100 * MSZ * MSZ;

enum class DIRECTION { 
	TOP, BOTTOM, LEFT, RIGHT, 
	DIAG_TL, DIAG_BL, 
	DIAG_TR, DIAG_BR };

// entities
const double SPEED = 0.0001;

// bullet
const double BULLET_BASE_DAMAGE = 2;

// grenade
const int NUM_BULLETS = 25;
const int TICK_TIME = 500; // time till explosion

// actor
const int SHOOT_CD = 150; // shooting cooldown time
const int GRENADE_CD = 5000; // grenade cooldown time
const int SEEK_CD = 1000;  // pathfinding cooldown time
const int HIDE_CD = 500;  // reposition cooldown time
const int BASE_HP = 100;
const int BASE_ARMOR = 20;
const int BASE_BULLET_AMMO = 20;
const int BASE_GRENADE_AMMO = 2;
const int BASE_HP_THRESHOLD = 20;

// teams
const int TEAM_A = 300;
const int TEAM_B = 301;
const int TEAM_SIZE = 2;

// danger map
const double DANGER_DELTA = 0.00025;

enum class HEURISTIC { ROOM, ENEMY, BOX };

// drawing scale
const double SCALE = 2.0 / MSZ;
const double ESCALE = SCALE / 2.0;

#endif /* __CONST_H__ */
