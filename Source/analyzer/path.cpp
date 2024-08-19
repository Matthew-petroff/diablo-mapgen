#include "path.h"

#include <iostream>

#include "../../types.h"

#include "item.h"
#include "../funkMapGen.h"
#include "../objects.h"
#include "../path.h"
#include "../quests.h"

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

int PathLength(Point start, Point end)
{
	return FindPath(PosOkPlayer, 0, start.x, start.y, end.x, end.y, Path);
}

/**
 * Unused
 */
bool IsVisible(Point start, Point end)
{
	if (start == Point { -1, -1 } || end == Point { -1, -1 }) {
		return false;
	}

	int horizontal = end.x - start.x + 10;
	int vertical = end.y - start.y + 10;

	if (horizontal < 0 || horizontal > MAXVIEWX)
		return false;
	if (vertical < 0 || vertical > MAXVIEWY)
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
		cDistance = 5;

	/** @todo Take teleport limits in to considerations instead of just estimating 5 steps */
	return cDistance * ticksToTeleport / 5;
}

int GetShortestTeleportTime(Point startA, Point startB, Point end)
{
	int teleportTime = GetTeleportTime(startA, end);
	int teleportTimePrevious = GetTeleportTime(startB, end);
	if (teleportTime == -1)
		teleportTime = teleportTimePrevious;
	if (teleportTime == -1)
		return -1;

	return std::min(teleportTime, teleportTimePrevious);
}

void setDoorSolidState(BOOLEAN doorState)
{
	if (leveltype == DTYPE_CATHEDRAL) {
		nSolidTable[44] = doorState;
		nSolidTable[46] = doorState;
		nSolidTable[51] = doorState;
		nSolidTable[56] = doorState;
		nSolidTable[214] = doorState;
		nSolidTable[270] = doorState;
	} else if (leveltype == DTYPE_CATACOMBS) {
		nSolidTable[55] = doorState;
		nSolidTable[58] = doorState;
		nSolidTable[538] = doorState;
		nSolidTable[540] = doorState;
	} else if (leveltype == DTYPE_CAVES) {
		nSolidTable[531] = doorState;
		nSolidTable[534] = doorState;
	}
}

bool IsGoodLevelSoursororStrategy()
{
	int tickLenth = 0;
	tickLenth += 20; // Load screens

	if (currlevel < 9) {
		int walkTicks = GetWalkTime(Spawn, StairsDown);
		if (walkTicks == -1) {
			if (Config.verbose)
				std::cerr << "Path: Gave up on walking to the stairs" << std::endl;
			return false;
		}
		tickLenth += walkTicks;
	} else if (currlevel == 9) {
		LocateItem();
		int pathToPuzzler = -1;
		if (POI != Point { -1, -1 }) {
			int walkTicks = GetWalkTime(Spawn, POI);
			if (walkTicks != -1) {
				int teleportTime = GetTeleportTime(Spawn, StairsDown);
				if (teleportTime != -1) {
					pathToPuzzler += walkTicks;
					pathToPuzzler += 40; // Pick up Puzzler
					pathToPuzzler += teleportTime;
				}
			}
		}

		if (pathToPuzzler != -1) {
			if (Config.verbose)
				std::cerr << "Path: Found Naj's Puzzler" << std::endl;
			tickLenth += pathToPuzzler;
		} else {
			if (Config.verbose)
				std::cerr << "Path: Went to town to get a book of teleport" << std::endl;
			tickLenth += 880; // Buying a book of teleport
			int walkTicks = GetTeleportTime(Spawn, StairsDown);
			if (walkTicks == -1) {
				if (Config.verbose)
					std::cerr << "Path: Couldn't find the stairs" << std::endl;
				return false;
			}
			tickLenth += walkTicks;
		}

		tickLenth += 1100; // Fight monsters to get Puzzler or level up to read the book
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

		// Locate Lazarous warp
		if (POI != Point { -1, -1 }) {
			if (Config.verbose)
				std::cerr << "Path: Found the warp to Lazarus" << std::endl;
			target = POI;
		}

		int teleportTime = GetShortestTeleportTime(Spawn, StairsDownPrevious, target);
		if (teleportTime == -1) {
			if (Config.verbose)
				std::cerr << "Path: Couldn't find the stairs" << std::endl;
			return false;
		}
		tickLenth += teleportTime;

		if (POI == Point { -1, -1 }) {
			if (Config.verbose)
				std::cerr << "Path: Told Cain about Lazarus" << std::endl;
			tickLenth += 20;  // Pick up staff
			tickLenth += 580; // Show staff to Cain
		}

		tickLenth += 460; // Defeat Lazarus

		int teleportTime2 = GetTeleportTime(target, StairsDown);
		if (teleportTime2 == -1) {
			if (Config.verbose)
				std::cerr << "Path: Couldn't find the stairs" << std::endl;
			return false;
		}
		tickLenth += teleportTime2;
	} else {
		int teleportTime = GetShortestTeleportTime(Spawn, StairsDownPrevious, StairsDown);
		if (teleportTime == -1) {
			if (Config.verbose)
				std::cerr << "Path: Couldn't find the stairs" << std::endl;
			return false;
		}
		tickLenth += teleportTime;

		if (currlevel == 16) {
			tickLenth += 180; // Defeat Diablo
		}
	}

	TotalTickLenth += tickLenth;

	if (Config.verbose)
		std::cerr << "Path: Compleated dlvl " << (int)currlevel << " @ " << formatTime() << std::endl;

	if (TotalTickLenth > *Config.target * 20) {
		if (Config.verbose)
			std::cerr << "Path: It's to slow to beat this one, giving up" << std::endl;
		return false;
	}

	return true;
}

bool IsGoodLevel()
{
	setDoorSolidState(FALSE); // Open doors
	bool result = IsGoodLevelSoursororStrategy();
	setDoorSolidState(TRUE); // Close doors

	return result;
}

bool Ended;

}

bool ScannerPath::skipSeed()
{
	if (quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
		if (Config.verbose)
			std::cerr << "Game Seed: " << sgGameInitInfo.dwSeed << " thrown out: Sign Quest" << std::endl;
		return true;
	}

	TotalTickLenth = 0;
	TotalTickLenth += 435;  // Walk to Adria
	TotalTickLenth += 1400; // Dup gold
	TotalTickLenth += 250;  // Buy to books
	TotalTickLenth += 200;  // Buy full manas
	TotalTickLenth += 540;  // Walk to church
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
		std::cout << sgGameInitInfo.dwSeed << " (etc " << formatTime() << ")" << std::endl;
		Ended = true;
	}

	return true;
}
