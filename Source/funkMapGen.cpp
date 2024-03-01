#include <algorithm>
#include <iostream>
#include <iomanip>

#include "../types.h"

#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "drlg_l4.h"
#include "engine.h"
#include "gendung.h"
#include "items.h"
#include "level.h"
#include "monster.h"
#include "objects.h"
#include "quests.h"
#include "themes.h"
#include "trigs.h"
#include "path.h"

#define MAXVIEWX 21
#define MAXVIEWY 21
bool isVisible[MAXVIEWY][MAXVIEWX] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //	-y
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },// -x	origin(10,10)	+x
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

typedef struct Point {
	int x;
	int y;

	bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Point &other) const
	{
		return !(*this == other);
	}
} Point;

static int InitLevelType(int l)
{
	if (l >= 1 && l <= 4)
		return DTYPE_CATHEDRAL;
	if (l >= 5 && l <= 8)
		return DTYPE_CATACOMBS;
	if (l >= 9 && l <= 12)
		return DTYPE_CAVES;

	return DTYPE_HELL;
}

Point Spawn;
Point StairsDown;
Point StairsDownPrevious;
char Path[MAX_PATH_LENGTH];
/**
 * @brief GET MAIN SEED, GET ALL MAP SEEDS
 * @return nothing, but updates RNG seeds list glSeedTbl[i]
 */
void seedSelection(uint32_t seed)
{
	SetRndSeed(0);
	sgGameInitInfo.dwSeed = seed;
	sgGameInitInfo.bDiff = gnDifficulty;

	gnDifficulty = sgGameInitInfo.bDiff;
	SetRndSeed(sgGameInitInfo.dwSeed);

	for (int i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = GetRndSeed();
		gnLevelTypeTbl[i] = InitLevelType(i);
	}
}

void createItemsFromObject(int oid)
{
	switch (object[oid]._otype) {
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		SetRndSeed(object[oid]._oRndSeed);
		if (setlevel) {
			for (int j = 0; j < object[oid]._oVar1; j++) {
				CreateRndItem(object[oid]._ox, object[oid]._oy, TRUE, TRUE, FALSE);
			}
		} else {
			for (int j = 0; j < object[oid]._oVar1; j++) {
				if (object[oid]._oVar2 != 0)
					CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
				else
					CreateRndUseful(0, object[oid]._ox, object[oid]._oy, TRUE);
			}
		}
		break;
	case OBJ_SARC:
		SetRndSeed(object[oid]._oRndSeed);
		if (object[oid]._oVar1 <= 2)
			CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_DECAP:
		SetRndSeed(object[oid]._oRndSeed);
		CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_BARREL:
		SetRndSeed(object[oid]._oRndSeed);
		if (object[oid]._oVar2 <= 1) {
			if (object[oid]._oVar3 == 0)
				CreateRndUseful(0, object[oid]._ox, object[oid]._oy, TRUE);
			else
				CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		}
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		SetRndSeed(object[oid]._oRndSeed);
		if (random_(161, 5) != 0)
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_SCROLL, TRUE, FALSE);
		else
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		SetRndSeed(object[oid]._oRndSeed);
		CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR: {
		SetRndSeed(object[oid]._oRndSeed);
		BOOL uniqueRnd = random_(0, 2);
		if (currlevel <= 5) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, ITYPE_LARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 6 && currlevel <= 9) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, uniqueRnd, ITYPE_MARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 10 && currlevel <= 12) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 13 && currlevel <= 16) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		}
	} break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK: {
		SetRndSeed(object[oid]._oRndSeed);
		int weaponType;

		switch (random_(0, 4) + ITYPE_SWORD) {
		case ITYPE_SWORD:
			weaponType = ITYPE_SWORD;
			break;
		case ITYPE_AXE:
			weaponType = ITYPE_AXE;
			break;
		case ITYPE_BOW:
			weaponType = ITYPE_BOW;
			break;
		case ITYPE_MACE:
			weaponType = ITYPE_MACE;
			break;
		}

		if (leveltype > 1)
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, weaponType, IMISC_NONE, TRUE, FALSE);
		else
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, weaponType, IMISC_NONE, TRUE, FALSE);
	} break;
	}
}

void printAsciiLevel()
{
	bool steps[MAXDUNX][MAXDUNY];

	for (int i = 0; i < MAXDUNY; ++i) {
		for (int j = 0; j < MAXDUNX; ++j) {
			steps[i][j] = false;
		}
	}

	Point position = Spawn;
	steps[position.x][position.y] = true;

	const char pathxdir[9] = { 0, 0, -1, 1, 0, -1, 1, 1, -1 };
	const char pathydir[9] = { 0, -1, 0, 0, 1, -1, -1, 1, 1 };

	for (int i = 0; i < MAX_PATH_LENGTH; ++i) {
		if (Path[i] == 0)
			break;
		position.x += pathxdir[Path[i]];
		position.y += pathydir[Path[i]];
		steps[position.x][position.y] = true;
	}

	for (int boby = 16; boby < MAXDUNY - 17; boby++) {
		for (int bobx = 16; bobx < MAXDUNX - 17; bobx++) {
			if (Spawn.x == bobx && Spawn.y == boby)
				std::cout << "▲";
			else if (StairsDown.x == bobx && StairsDown.y == boby)
				std::cout << "▼";
			else if (steps[bobx][boby])
				std::cout << "=";
			else if (nSolidTable[dPiece[bobx][boby]])
				std::cout << "#";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void printHelp()
{
	std::cout << "--help         Print this message and exit" << std::endl;
	std::cout << "--quiet        Do not print to console" << std::endl;
	std::cout << "--export       Export levels as .dun files" << std::endl;
	std::cout << "--start <#>    The seed to start from" << std::endl;
	std::cout << "--count <#>    The number of seeds to process" << std::endl;
	std::cout << "--quality <#>  Number of levels that must be good (default 4)" << std::endl;
	std::cout << "--verbose      Print out details about rejected seeds" << std::endl;
}

extern int sglGameSeed;

#define TEMPLATEX 8
#define TEMPLATEY 8

const BYTE GROOBO2[TEMPLATEX][TEMPLATEY] = {
	// clang-format off
    {   0,  0,  0,  0,  8,  0,  0,  0 },
    {   0,  8,  8, 51, 50,  8,  8, 89 },
    {   8,  8, 11, 48, 49,  3, 11, 88 },
    {   8,  8, 47,106,  7, 13, 12,106 },
    {   8,  8, 46,107,  7,106,108,106 },
    {   0,  8,  1, 14,106,106,  5, 14 },
    {   0,  0,  8,  1, 14,106,  4,  0 },
    {   0,  0,  0,  0, 12,  0,  0,  0 },
    // clang-format on
};

const BYTE GROOBO[TEMPLATEX][TEMPLATEY] = {
	// clang-format off
    {   0,  0,  0,  0,  8,  0,  0,  0 },
    {   0,  8,  8, 51, 50,  8,  8, 89 },
    {   8,  8, 11, 48, 49,  3, 11, 88 },
    {   8,  8, 47,106,  7, 13, 12,106 },
    {   8,  8, 46,107,  7,106,108,106 },
    {   0,  8,  1, 14,106,106,  5, 14 },
    {   0,  0,  8,  1, 14,106,  4,  0 },
    {   0,  0,  0,  0, 12,  0,  0,  0 },
    // clang-format on
};

bool MatchGroobo() {
	bool foundStairs = false;
    int sx = 0;
    int sy = 0;
    for (int y = 0; y < DMAXX && !foundStairs; y++) {
        for (int x = 0; x < DMAXY; x++) {
			if (dungeon[x][y] == 51) {
				sx = x - 3;
				sy = y - 1;
				foundStairs = true;
				break;
			}
        }
    }

    if (sx < 0 || sy < 0 || sx >= DMAXX - TEMPLATEX || sx >= DMAXX - TEMPLATEX)
		return false;

	bool found = true;
	for (int column = 0; column < TEMPLATEX; column++) {
		for (int row = 0; row < TEMPLATEY; row++) {
			if (GROOBO[row][column] == 0) continue;
			if (dungeon[sx + column][sy + row] != GROOBO[row][column]) {
				found = false;
				break;
			}
		}
		if (!found) return false;
	}

    return true;
}

int main(int argc, char **argv)
{
	uint32_t startSeed = 0;
	uint32_t seedCount = 1;
	bool quiet = false;
	bool exportLevels = false;
	int quality = 4;
	bool verbose = false;

	for (int i = 0; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--help") {
			printHelp();
			return 0;
		} else if (arg == "--quiet") {
			quiet = true;
		} else if (arg == "--export") {
			exportLevels = true;
		} else if (arg == "--start" && argc >= i + 1) {
			startSeed = std::stoll(argv[i + 1]);
		} else if (arg == "--count" && argc >= i + 1) {
			seedCount = std::stoll(argv[i + 1]);
		} else if (arg == "--quality" && argc >= i + 1) {
			quality = std::stoi(argv[i + 1]);
		} else if (arg == "--verbose") {
			verbose = true;
		}
	}

	currlevel = 9;
	leveltype = DTYPE_CAVES;

	int seconds = time(NULL);
	uint32_t prevseed = startSeed;
	for (uint32_t seed = startSeed; seed < startSeed + seedCount; seed++) {
		int elapsed = time(NULL) - seconds;
		if (elapsed >= 10) {
			int pct = 100 * (seed - startSeed) / seedCount;
			int speed = ((seed - prevseed) / 10);
			int eta = (seedCount - (seed - startSeed)) / speed;
			std::cerr << "processing seed " << seed << " (" << pct << "% eta: " << eta << "s) " << speed << "seed/s" << std::endl;
			seconds += elapsed;
			prevseed = seed;
		}

		glSeedTbl[9] = seed;

		if (!CreateL3Dungeon(seed, 0) || !MatchGroobo())
			continue;

		std::cout << "Game seed: " << seed << std::endl;

		if (!quiet)
			printAsciiLevel();
		if (exportLevels)
			ExportDun(seed);
	}

	return 0;
}
