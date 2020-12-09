#include "Actor.h"
#include "glut.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>
#include "AmmoBox.h"
#include "HealthBox.h"
#include "logic.h"
#include "Grenade.h"

Actor::Actor(double x, double y, int team, int maze[][MSZ], double maxHp, double armor, double hpThreshold, int maxBulletAmmo, int maxGrenadeAmmo) :
	MovingEntity(x, y, 0, 0, team, SPEED * 2.5, false),
	hp(maxHp), maxHp(maxHp), armor(armor), hpThreshold(hpThreshold),
	bulletAmmo(maxBulletAmmo), maxBulletAmmo(maxBulletAmmo),
	grenadeAmmo(maxGrenadeAmmo), maxGrenadeAmmo(maxGrenadeAmmo),
	state(eState::IDLE), lastShotTime(-1), lastThrowTime(-1), lastSeekTime(-1), lastHideTime(-1)
{
	// copy maze for pathfinding
	std::copy(&maze[0][0], &maze[0][0] + MSZ * MSZ, &mazeCopy[0][0]);

	// extra stat points
	int totalExtraPoints = 30;

	// randomize extra points
	int extraAmmo = rand() % 6;
	totalExtraPoints -= extraAmmo * 2;

	int extraHp = rand() % totalExtraPoints;
	totalExtraPoints -= extraHp;
	int extraArmor = totalExtraPoints / 2;

	// distrubute extra stats
	bulletAmmo = maxBulletAmmo += extraAmmo;
	hp = maxHp += extraHp;
	armor += extraArmor;

	hpThreshold += rand() % 10;

	if (DEBUG)
	{
		std::cout << "Actor:" << endl;
		std::cout << "HP: " << maxHp << endl;
		std::cout << "ARMOR: " << armor << endl;
		std::cout << "AMMO: " << maxBulletAmmo << endl;
		std::cout << "HP THRESHOLD: " << hpThreshold << endl << endl;
	}
}

void Actor::render(int maze[][MSZ])
{
	if (team == TEAM_A)
		glColor3d(0.2, 0.2, 0.8);
	else
		glColor3d(0.8, 0.2, 0.2);

	if (state == eState::DEAD)
	{
		// draw halo
		glColor3d(0.7, 0.7, 0.3);
		glBegin(GL_LINES);
		glVertex2d(x + 0.015, y + 0.02);
		glVertex2d(x - 0.015, y + 0.02);
		glEnd();

		if (team == TEAM_A)
			glColor3d(0.5, 0.5, 0.8);
		else
			glColor3d(0.8, 0.5, 0.5);
	}

	// draw actor
	glBegin(GL_POLYGON);
	glVertex2d(x, y + 0.012);
	glVertex2d(x + 0.008, y + 0.008);
	glVertex2d(x + 0.012, y);
	glVertex2d(x + 0.008, y - 0.008);
	glVertex2d(x, y - 0.012);
	glVertex2d(x - 0.008, y - 0.008);
	glVertex2d(x - 0.012, y);
	glVertex2d(x - 0.008, y + 0.008);
	glEnd();

	if (state != eState::DEAD)
	{
		// draw hp bar
		double hPer = hp / maxHp;
		glColor3d(1, 0, 0);
		glBegin(GL_LINES);
		glVertex2d(x - 0.02, y - 0.015);
		glVertex2d(x - 0.02, y - 0.015 + 0.03 * hPer);
		glEnd();

		// draw ammo bar
		double aPer = bulletAmmo / (double)maxBulletAmmo;

		glColor3d(0, 0, 1);
		glBegin(GL_LINES);
		glVertex2d(x + 0.02, y - 0.015);
		glVertex2d(x + 0.02, y - 0.015 + 0.03 * aPer);
		glEnd();
	}
}

void Actor::move(int maze[][MSZ])
{
	if (state == eState::DEAD)
		return;
	
	int currentRow = getRow();
	int currentCol = getCol();

	if (moving)
	{
		x += dx * speed;
		y += dy * speed;
	}

	int nextRow = getRow();
	int nextCol = getCol();

	// remove passed tile from pathfinding
	if (currentRow != nextRow || currentCol != nextCol)
		mazeCopy[currentRow][currentCol] = SPACE;
}

void Actor::shoot(double dx, double dy, std::vector<Bullet>& bullets)
{
	lastShotTime = (lastShotTime + 1) % SHOOT_CD;
	if (!lastShotTime && bulletAmmo>0) // check shooting cooldown
	{
		bullets.push_back(Bullet(x, y, dx, dy, team));
		bulletAmmo -= 1;
	}
}

void Actor::throwGrenade(double dx, double dy, double tx, double ty, std::vector<Grenade>& grenades)
{
	lastThrowTime = (lastThrowTime + 1) % GRENADE_CD;
	if (!lastThrowTime && grenadeAmmo > 0) // check grenade cooldown
	{
		grenades.push_back(Grenade(x, y, dx, dy, tx, ty, team));
		grenadeAmmo -= 1;
	}
}

void Actor::act(int maze[][MSZ], double dangerMap[][MSZ], Room rooms[NUM_ROOMS], std::vector<Actor*>& actors,
	std::vector<LootableBox*>& boxes, std::vector<Bullet>& bullets, std::vector<Grenade>& grenades)
{
	bool enemyInRoom = false;
	Actor* enemy = nullptr;
	LootableBox* box = nullptr;
	LootableBox* b = nullptr;

	if (state == eState::DEAD) // actor is dead
		return;

	if (getCurrentRoom(rooms) == -1) // in room transition, finish moving to next room
	{
		orient();
		move(maze);
		return;
	}

	if (state == eState::MOVE_TO_BOX || state == eState::MOVE_TO_ENEMY || state == eState::MOVE_TO_HIDING) // moving to location
	{
		if (state == eState::MOVE_TO_ENEMY)
		{
			lastSeekTime = (lastSeekTime + 1) % SEEK_CD;
			if (!lastSeekTime) // check pathfinding cooldown
				state = eState::IDLE;
		}

		orient();
		move(maze);
	}
	else if (state == eState::HIDE)
	{
		lastHideTime = (lastHideTime + 1) % HIDE_CD;
		if (!lastHideTime) // check reposition cooldown
		{
			lastHideTime %= (HIDE_CD / 2);
			state = eState::IDLE;
		}
	}
	else if (state == eState::LOOT) // loot box in current tile
	{
		if (DEBUG)
			std::cout << "LOOTING! " << std::endl;

		for (LootableBox* bp : boxes)
			if (bp->getRow() == getRow() && bp->getCol() == getCol())
				bp->loot(*this);

		state = eState::IDLE;
		moving = false;
	}

	if ((state == eState::IDLE || state == eState::HIDE)
		&& (bulletAmmo == 0 || hp <= hpThreshold)) // no ammo or low hp
	{
		if (DEBUG)
			std::cout << "PATHFINDING TO BOX" << std::endl;

		// find box
		for (LootableBox* bp : boxes)
		{
			if (hp <= hpThreshold)
				b = dynamic_cast<HealthBox*>(bp);
			else if (bulletAmmo == 0)
				b = dynamic_cast<AmmoBox*>(bp);

			if (b)
			{
				if (box) // found earlier box
				{
					if (b->getDist(x, y) < box->getDist(x, y)) // box is closer than the last
						box = b;
				}
				else // first box seen
					box = b;
			}
		}
		// find path to box using A*
		if (box)
			findPath(box->getRow(), box->getCol(), HEURISTIC::BOX);
	}

	// check if enemy in current room
	for (Actor* a : actors)
	{
		if (a != this && a->team != team && a->isAlive())
		{
			if (a->getCurrentRoom(rooms) == getCurrentRoom(rooms)) // found enemy in current room
			{
				enemyInRoom = true;

				// get angle to enemy
				double alpha = getAngle(a->x, a->y);
				double dx = cos(alpha);
				double dy = sin(alpha);

				// check LOS to target
				Bullet b(x, y, dx, dy, team, true, false);
				if (b.hasLineOfSight(maze, a->x, a->y)) // has LOS, shoot
				{
					// mild shooting accuracy deviation
					double rnd_x = (rand() % 300 - 150.0) * 0.001;
					double rnd_y = (rand() % 300 - 150.0) * 0.001;

					if (bulletAmmo > 0)
						shoot( dx + rnd_x, dy + rnd_y, bullets);

					if((rand() + bulletAmmo + (int)hp) % 1000 == 0 || bulletAmmo == 0)
						throwGrenade(dx + rnd_x, dy + rnd_y, a->x, a->y, grenades);
				}

				if (state == eState::IDLE && getCurrentRoom(rooms) != -1) // hide
				{
					if (DEBUG)
						std::cout << "HIDING" << std::endl;
					hide(dangerMap, rooms);
				}
			}
			else if (!enemyInRoom) // no enemy in current room so far
			{
				if (a->getCurrentRoom(rooms) != -1)
				{
					if (enemy) // found earlier enemy
					{
						if (a->getDist(x, y) < enemy->getDist(x, y)) // enemy is closer than the last
							enemy = a;
					}
					else // first enemy seen
						enemy = a;
				}
			}
		}
	}


	if (state == eState::IDLE && !enemyInRoom && enemy) // found no enemy in room, actor not busy
	{
		// find path to enemy using A*
		findPath(enemy->getRow(), enemy->getCol(), HEURISTIC::ENEMY);
		if (DEBUG)
			std::cout << "PATHFINDING TO ENEMY" << std::endl;
	}
}


void Actor::kill()
{
	hp = 0;
	moving = false;
	state = eState::DEAD;
}

void Actor::clearPathing()
{
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			if (mazeCopy[i][j] != WALL)
				mazeCopy[i][j] = SPACE;
}

void Actor::findPath(int row, int col, HEURISTIC hr)
{
	int sRow = getRow();
	int sCol = getCol();

	clearPathing();

	mazeCopy[sRow][sCol] = START;
	mazeCopy[row][col] = TARGET;

	Node* pn = new Node(sRow, sCol, nullptr, 0, distance(sCol, sRow, col, row));
	aStar(col, row, mazeCopy, pn, TARGET, START, PATH, true, hr);

	if (hr == HEURISTIC::ENEMY)
		state = eState::MOVE_TO_ENEMY;
	else if (hr == HEURISTIC::BOX)
		state = eState::MOVE_TO_BOX;

	moving = true;
}

void Actor::hide(double dangerMap[][MSZ], Room rooms[NUM_ROOMS])
{
	Room room = rooms[getCurrentRoom(rooms)];
	int i = 0;
	int j = 0;
	int rcw = room.getWidth();
	int rch = room.getHeight();
	int rcx = room.getCenterX() - rcw / 2;
	int rcy = room.getCenterY() - rch / 2;

	double probSum = 0;

	// weighted probability based on tile safety
	for (i = rcy; i < rch + rcy; i++)
		for (j = rcx; j < rcw + rcx; j++)
			probSum += pow((1 - dangerMap[i][j]), 2);

	double rng = rand() % (int)(probSum * 1000) / 1000.0;

	double walkSum = 0;

	for (i = rcy; i < rch + rcy; i++)
	{
		for (j = rcx; j < rcw + rcx; j++)
		{
			walkSum += pow((1 - dangerMap[i][j]), 2);
			if (walkSum > rng)
				break;
		}
		if (walkSum > rng)
			break;
	}

	clearPathing();

	mazeCopy[getRow()][getCol()] = START;
	mazeCopy[i][j] = TARGET;

	int sRow = getRow();
	int sCol = getCol();

	Node* pn = new Node(sRow, sCol, nullptr, 0, distance(sCol, sRow, j, i));
	aStar(j, i, mazeCopy, pn, TARGET, START, PATH, true, HEURISTIC::ENEMY);

	state = eState::MOVE_TO_HIDING;
	moving = true;
}

bool Actor::isAlive()
{
	if (state != eState::DEAD)
		return true;
	return false;
}

void Actor::hit(Bullet &b)
{
	double dmg = BULLET_BASE_DAMAGE * (2.5 - b.getTravelledDistance());
	hp -= (100 - armor) * dmg / 100;
	if (hp <= 0)
		kill();

	if(DEBUG)
		std::cout << "DMG: " << (100 - armor) * dmg / 100 << std::endl;
}

bool Actor::isHit(Bullet& b)
{
	if (b.getTeam() == team)
		return false;

	if (std::abs(b.getX() - x) < 0.015)
		if (std::abs(b.getY() - y) < 0.015)
			return true;

	return false;
}

int Actor::getCurrentRoom(Room rooms[NUM_ROOMS])
{
	int cy = getRow();
	int cx = getCol();

	for (int i = 0; i < NUM_ROOMS; i++)
		if (rooms[i].overlap(1, 1, cx, cy))
			return i;

	return -1;
}

double Actor::getAngle(double x, double y)
{
	double delta_x = x - this->x;
	double delta_y = y - this->y;
	return atan2(delta_y, delta_x);
}

void Actor::orient()
{
	int row = getRow();
	int col = getCol();

	//double e = speed * 15;

	//// align to room center
	//if (moving && (dx != 0 || dy != 0) && mazeCopy[row][col] != WALL)
	//	if (!(std::abs(x - getCenterX()) < e && std::abs(y - getCenterY()) < e))
	//		return;

	if (mazeCopy[row + 1][col] == PATH) // step up
	{
		dx = 0;
		dy = 1;
	}
	else if (mazeCopy[row - 1][col] == PATH) // step down
	{
		dx = 0;
		dy = -1;
	}
	else if (mazeCopy[row][col + 1] == PATH) // step right
	{
		dx = 1;
		dy = 0;
	}
	else if (mazeCopy[row][col - 1] == PATH) // step left
	{
		dx = -1;
		dy = 0;
	}
	else if (mazeCopy[row + 1][col - 1] == PATH) // step top left
	{
		dx = -0.7071;
		dy = 0.7071;
	}
	else if (mazeCopy[row - 1][col - 1] == PATH) // step bottom left
	{
		dx = -0.7071;
		dy = -0.7071;
	}
	else if (mazeCopy[row + 1][col + 1] == PATH) // step top right
	{
		dx = 0.7071;
		dy = 0.7071;
	}
	else if (mazeCopy[row - 1][col + 1] == PATH) // step bottom right
	{
		dx = 0.7071;
		dy = -0.7071;
	}
	else // reached target
	{
		if (state == eState::MOVE_TO_BOX)
			state = eState::LOOT;
		else if (state == eState::MOVE_TO_HIDING)
			state = eState::HIDE;
		else if (state == eState::MOVE_TO_ENEMY)
			state = eState::IDLE;

		moving = false;
	}
}

void Actor::refillBullets()
{
	bulletAmmo = maxBulletAmmo;
}

void Actor::refillGrenades()
{
	grenadeAmmo = maxGrenadeAmmo;
}

void Actor::restoreHealth()
{
	hp = maxHp;
}
