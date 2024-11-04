/**
 * @file analyzer/path.cpp
 *
 * Implementation of scanner for finding the optimal path through the game.
 */

#include "path.h"

#include <iostream>

#include "../../types.h"

#include "item.h"
#include "../funkMapGen.h"
#include "../objects.h"
#include "../path.h"
#include "../quests.h"
#include "../monster.h"

namespace {

#define MAXVIEWX 21
#define MAXVIEWY 21

bool isVisible[MAXVIEWY][MAXVIEWX] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //	-y
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // -x	origin(10,10)	+x
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //	+y
};

bool isVisiblePhasing[MAXVIEWY][MAXVIEWX] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //	-y
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -x	origin(10,10)	+x
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //	+y
};

Point StairsDownPrevious;

BOOL PosOkPlayer(int pnum, int x, int y)
{
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY)
		return FALSE;
	int tileId = dPiece[x][y];
	if (tileId == 0)
		return FALSE;
	if (nSolidTable[tileId])
		return FALSE;

	if (dObject[x][y] != 0) {
		ObjectStruct *obj = &object[abs(dObject[x][y]) - 1];
		if (obj->_oSolidFlag && !obj->_oBreak) {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL PosOkGhost(int pnum, int x, int y)
{
	return TRUE;
}

int PathLength(Point start, Point end)
{
	return FindPath(PosOkPlayer, 0, start.x, start.y, end.x, end.y, Path);
}

int IndicatePath(Point start, Point end)
{
	return FindPath(PosOkGhost, 0, start.x, start.y, end.x, end.y, Path);
}

bool IsVisible(Point start, Point end)
{
	if (start == Point { -1, -1 } || end == Point { -1, -1 }) {
		return false;
	}

	int horizontal = end.x - start.x + 10;
	int vertical = end.y - start.y + 10;

	if (horizontal < 0 || horizontal >= MAXVIEWX)
		return false;
	if (vertical < 0 || vertical >= MAXVIEWY)
		return false;
	return isVisible[vertical][horizontal];
}

int TotalTickLenth;

std::string formatTime()
{
	float time = (float)TotalTickLenth / 20;
	int min = time / 60;
	char fmt[12];
	sprintf(fmt, "%u:%05.2lf", min, time - min * 60);

	return fmt;
}

int CalcStairsChebyshevDistance(Point start, Point end)
{
	if (start == Point { -1, -1 } || end == Point { -1, -1 })
		return -1;

	int horizontal = std::max(start.x, end.x) - std::min(start.x, end.x);
	int vertical = std::max(start.y, end.y) - std::min(start.y, end.y);

	return std::max(horizontal, vertical);
}

int GetWalkTime(Point start, Point end)
{
	constexpr int ticksToWalkATile = 8;

	int cDistance = CalcStairsChebyshevDistance(start, end);
	if (cDistance == -1 || cDistance > MAX_PATH_LENGTH)
		return -1;

	int stairsPath = PathLength(start, end);
	if (stairsPath == 0)
		return -1;

	return stairsPath * ticksToWalkATile;
}

int GetTeleportTime(Point start, Point end)
{
	constexpr int ticksToTeleport = 12;

	int cDistance = CalcStairsChebyshevDistance(start, end);
	if (cDistance == -1) {
		return -1;
	}

	if (IsVisible(start, end))
		cDistance = 7;

	/** @todo Take teleport limits into consideration instead of just estimating 5 steps */
	return cDistance * ticksToTeleport / 7;
}

int GetShortestTeleportTime(Point startA, Point startB, Point end)
{
	int teleportTime = GetTeleportTime(startA, end);
	int teleportTimePrevious = GetTeleportTime(startB, end);
	if (teleportTime == -1 && teleportTimePrevious == -1)
		return -1;

	if (teleportTime > -1 && teleportTime <= teleportTimePrevious) {
		IndicatePath(startA, end);
		return teleportTime;
	}

	std::cout << "PATH: Using load teleporting" << std::endl;
		IndicatePath(startB, end);
		Spawn = StairsDownPrevious;
	return teleportTimePrevious;
}

int GetShortestPhasingTime(Point start, Point end)
{
	constexpr int ticksToTeleport = 12;

	int cDistance = CalcStairsChebyshevDistance(start, end);
	if (cDistance == -1) {
		return -1;
	}

	/** @todo Take phasing limits into consideration instead of just estimating based on distance */
	return cDistance * (ticksToTeleport + 3) / 5 + 20;
}

void setDoorSolidState(BOOLEAN doorState)
{
	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		nSolidTable[44] = doorState;
		nSolidTable[46] = doorState;
		nSolidTable[51] = doorState;
		nSolidTable[56] = doorState;
		nSolidTable[214] = doorState;
		nSolidTable[270] = doorState;
		break;
	case DTYPE_CATACOMBS:
		nSolidTable[55] = doorState;
		nSolidTable[58] = doorState;
		nSolidTable[538] = doorState;
		nSolidTable[540] = doorState;
		break;
	case DTYPE_CAVES:
		nSolidTable[531] = doorState;
		nSolidTable[534] = doorState;
		break;
	default:
		break;
	}
}

static void HighLightGear()
{
	Config.targetStr = "Book of Fire Wall";
	if (LocateItems(0, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (yellow)" << std::endl;
	}
	Config.targetStr = "Scroll of Identify";
	if (LocateItems(2, false, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (green)" << std::endl;
	}
	Config.targetStr = "Book of Mana Shield";
	if (LocateItems(1, false, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (cyan)" << std::endl;
	}
	Config.targetStr = "Scroll of Mana Shield";
	if (LocateItems(1, false, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (cyan)" << std::endl;
	}
	Config.targetStr = "Book of Stone Curse";
	if (LocateItems(3, false, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (gray)" << std::endl;
	}
	Config.targetStr = "Scroll of Stone Curse";
	if (LocateItems(3, false, false)) {
		if (Config.verbose)
			std::cout << "PATH: Located " << Config.targetStr << " (gray)" << std::endl;
	}
}

bool IsGoodLevelSorcStrategy()
{
	int tickLenth = 0;
	//tickLenth += 20; // Load screens

	if (currlevel < 3) {
		int walkTicks = GetWalkTime(Spawn, StairsDown);
		if (walkTicks == -1) {
			if (Config.verbose)
				std::cout << "PATH: Aborted walking to stairs." << std::endl;
			return false;
		}
		tickLenth += walkTicks;
	} else if (currlevel == 3) {
		Config.targetStr = "Scroll of Phasing";
		int pathToItem = -1;

		if (LocateItem(false)) {
			int walkTicks = GetWalkTime(Spawn, POI);
			if (walkTicks != -1) {
				int teleportTime = GetShortestPhasingTime(POI, StairsDown);
				if (teleportTime != -1) {
					pathToItem += walkTicks;
					pathToItem += 23; // Pick up target item
					pathToItem += teleportTime;
				}
			}
		}

		if (pathToItem != -1) {
			if (Config.verbose)
				std::cout << "PATH: Located " << Config.targetStr << std::endl;
			tickLenth += pathToItem;
		} else {
			if (Config.verbose)
				std::cout << "PATH: Went to town to get obtain Scroll of Phasing." << std::endl;
			tickLenth += 880; // Buying a Scroll of Phasing
			int walkTicks = GetShortestPhasingTime(Spawn, StairsDown);
			if (walkTicks == -1) {
				if (Config.verbose)
					std::cout << "PATH: Stairs not found." << std::endl;
				return false;
			}
			tickLenth += walkTicks;
		}
	} else if (currlevel == 8) {
		if (quests[Q_ZHAR]._qactive != QUEST_NOTAVAIL) {
			if (Config.verbose)
				std::cout << "PATH: Shopping at Zhar." << std::endl;
			Point target = { monster[4]._mx, monster[4]._my };
			int teleportTime = GetShortestPhasingTime(Spawn, target);
			if (teleportTime == -1) {
				if (Config.verbose)
					std::cout << "PATH: Zhar not found." << std::endl;
				return false;
			}
			tickLenth += teleportTime;
			tickLenth += 60; // Shop Zhar
			teleportTime = GetShortestPhasingTime(target, StairsDown);
			if (teleportTime == -1) {
				if (Config.verbose)
					std::cout << "PATH: Stairs not found." << std::endl;
				return false;
			}
			tickLenth += teleportTime;
			IndicatePath(Spawn, target);
		} else {
			int teleportTime = GetShortestPhasingTime(Spawn, StairsDown);
			if (teleportTime == -1) {
				if (Config.verbose)
					std::cout << "PATH: Stairs not found." << std::endl;
				return false;
			}
			tickLenth += teleportTime;
			IndicatePath(Spawn, StairsDown);
		}
	} else if (currlevel < 9) {
		int teleportTime = GetShortestPhasingTime(Spawn, StairsDown);
		if (teleportTime == -1) {
			if (Config.verbose)
				std::cout << "PATH: Stairs not found." << std::endl;
			return false;
		}
		tickLenth += teleportTime;
		IndicatePath(Spawn, StairsDown);
	} else if (currlevel == 9) {
		Config.targetStr = "Naj's Puzzler";
		int pathToItem = -1;

		if (LocateItem(false)) {
			int walkTicks = GetShortestPhasingTime(Spawn, POI);
			if (walkTicks != -1) {
				int teleportTime = GetTeleportTime(POI, StairsDown);
				if (teleportTime != -1) {
					pathToItem += walkTicks;
					pathToItem += 23; // Pick up target item
					pathToItem += teleportTime;
				}
			}
		}

		if (pathToItem != -1) {
			if (Config.verbose)
				std::cout << "PATH: Located " << Config.targetStr << std::endl;
			tickLenth += pathToItem;
			IndicatePath(Spawn, POI);
		} else {
			if (Config.verbose)
				std::cout << "PATH: Went to town to get obtain Book of Teleport." << std::endl;
			tickLenth += 880; // Buying a book of teleport
			int walkTicks = GetTeleportTime(Spawn, StairsDown);
			if (walkTicks == -1) {
				if (Config.verbose)
					std::cout << "PATH: Stairs not found." << std::endl;
				return false;
			}
			tickLenth += walkTicks;
			IndicatePath(Spawn, StairsDown);
		}
	} else if (currlevel == 15) {
		Point target = { -1, -1 };

		// Locate Lazarus staff
		for (int i = 0; i < nobjects; i++) {
			int oid = objectactive[i];
			ObjectStruct stand = object[oid];
			if (stand._otype != OBJ_LAZSTAND)
				continue;
			target = { stand._ox, stand._oy };
		}

		// Locate Lazarus warp
		if (POI != Point { -1, -1 }) {
			if (Config.verbose)
				std::cout << "PATH: Located Lazarus warp." << std::endl;
			target = POI;
		}

		int teleportTime = GetShortestTeleportTime(Spawn, StairsDownPrevious, target);
		if (teleportTime == -1) {
			if (Config.verbose)
				std::cout << "PATH: Stairs not found." << std::endl;
			return false;
		}
		tickLenth += teleportTime;

		if (POI == Point { -1, -1 }) {
			if (Config.verbose)
				std::cout << "PATH: Advanced Lazarus quest at Cain." << std::endl;
			tickLenth += 20;  // Pick up staff
			tickLenth += 580; // Show staff to Cain
		}

		tickLenth += 260; // Defeat Lazarus

		int teleportTime2 = GetTeleportTime(target, StairsDown);
		if (teleportTime2 == -1) {
			if (Config.verbose)
				std::cout << "PATH: Stairs not found." << std::endl;
			return false;
		}
		tickLenth += teleportTime2;
	} else {
		int teleportTime = GetShortestTeleportTime(Spawn, StairsDownPrevious, StairsDown);
		if (teleportTime == -1) {
			if (Config.verbose)
				std::cout << "PATH: Stairs not found." << std::endl;
			return false;
		}
		tickLenth += teleportTime;

		if (currlevel == 16) {
			tickLenth += 180; // Defeat Diablo
		}
	}

	TotalTickLenth += tickLenth;

	if (Config.verbose)
		std::cout << "PATH: Completed dungeon level " << (int)currlevel << " @ " << formatTime() << std::endl;

	if (TotalTickLenth > *Config.target * 20) {
		if (Config.verbose)
			std::cout << "PATH: Exceeded path time. Aborted." << std::endl;
		return false;
	}

	HighLightGear();

	return true;
}

bool IsGoodLevel()
{
	setDoorSolidState(FALSE); // Open doors
	bool result = IsGoodLevelSorcStrategy();
	setDoorSolidState(TRUE); // Close doors

	return result;
}

bool Ended;

}

/*
 * @brief Skips the game seed if the sign quest is present.
 */
bool ScannerPath::skipSeed()
{
	TotalTickLenth = 0;
	TotalTickLenth += 545;  // Walk to Cathedral
	//TotalTickLenth += 435;  // Walk to Adria
	//TotalTickLenth += 1400; // Dupe Gold
	//TotalTickLenth += 250;  // Buy 2 books
	//TotalTickLenth += 200;  // Buy Potions of Full Mana
	//TotalTickLenth += 540;  // Walk to Cathedral
	Ended = false;

	return false;
}

bool ScannerPath::skipLevel(int level)
{
	return Ended;
}

bool ScannerPath::levelMatches(std::optional<uint32_t> levelSeed)
{
	std::memset(Path, 0, sizeof(Path));
	if (!IsGoodLevel()) {
		Ended = true;
		return Config.verbose;
	}

	StairsDownPrevious = StairsDown;

	int level = currlevel;
	if (level == 16) {
		std::cout << sgGameInitInfo.dwSeed << " (" << formatTime() << ")" << std::endl;
		Ended = true;
	}

	return true;
}
