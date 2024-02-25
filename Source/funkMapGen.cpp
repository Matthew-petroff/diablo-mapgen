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

void whatleveltype()
{
	switch (currlevel) {
	case 1:
		leveltype = DTYPE_CATHEDRAL;
		break;
	case 5:
		leveltype = DTYPE_CATACOMBS;
		break;
	case 9:
		leveltype = DTYPE_CAVES;
		break;
	case 13:
		leveltype = DTYPE_HELL;
		break;
	default:
		return;
	}

	LoadLvlGFX();
	FillSolidBlockTbls();
}

Point Spawn;
Point StairsDown;
Point StairsDownPrevious;

void InitStairCordinates()
{
	if (leveltype == DTYPE_CATHEDRAL)
		InitL1Triggers();
	else if (leveltype == DTYPE_CATACOMBS)
		InitL2Triggers();
	else if (leveltype == DTYPE_CAVES)
		InitL3Triggers();
	else if (leveltype == DTYPE_HELL)
		InitL4Triggers();

	Spawn = { -1, -1 };
	StairsDown = { -1, -1 };

	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i]._tmsg == WM_DIABNEXTLVL) {
			StairsDown = { trigs[i]._tx, trigs[i]._ty };
		} else if (trigs[i]._tmsg == WM_DIABPREVLVL) {
			if (leveltype == DTYPE_CATHEDRAL)
				Spawn = { trigs[i]._tx + 1, trigs[i]._ty + 2 };
			else if (leveltype == DTYPE_CATACOMBS)
				Spawn = { trigs[i]._tx + 1, trigs[i]._ty + 1 };
			else if (leveltype == DTYPE_CAVES)
				Spawn = { trigs[i]._tx, trigs[i]._ty + 1 };
			else if (leveltype == DTYPE_HELL)
				Spawn = { trigs[i]._tx + 1, trigs[i]._ty };
		}
	}
}

BOOL PosOkPlayer(int pnum, int x, int y)
{
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY)
		return FALSE;
	int tileId = dPiece[x][y];
	if (tileId == 0)
		return FALSE;
	if (nSolidTable[tileId])
		return FALSE;

	// if (dObject[x][y] != 0) {
	//  char bv;
	//	if (dObject[x][y] > 0) {
	//		bv = dObject[x][y] - 1;
	//	} else {
	//		bv = -(dObject[x][y] + 1);
	//	}
	//	if (object[bv]._oSolidFlag) {
	//		return FALSE;
	//	}
	// }

	return TRUE;
}

char Path[MAX_PATH_LENGTH];

int PathLength()
{
	std::memset(Path, 0, sizeof(Path));

	return FindPath(PosOkPlayer, 0, Spawn.x, Spawn.y, StairsDown.x, StairsDown.y, Path);
}

int CalcStairsChebyshevDistance()
{
	if (Spawn == Point { -1, -1 } || StairsDown == Point { -1, -1 }) {
		return -1;
	}

	int horizontal = std::max(Spawn.x, StairsDown.x) - std::min(Spawn.x, StairsDown.x);
	int vertical = std::max(Spawn.y, StairsDown.y) - std::min(Spawn.y, StairsDown.y);

	return std::max(horizontal, vertical);
}

bool IsVisibleSpawn()
{
	if (Spawn == Point { -1, -1 } || StairsDown == Point { -1, -1 }) {
		return false;
	}

	int horizontal = StairsDown.x - Spawn.x + 10;
	int vertical = StairsDown.y - Spawn.y + 10;

	if (horizontal < 0 || horizontal > MAXVIEWX)
		return false;
	if (vertical < 0 || vertical > MAXVIEWY)
		return false;
	return isVisible[vertical][horizontal];
}

bool IsVisiblePrevious()
{
	if (StairsDownPrevious == Point { -1, -1 } || StairsDown == Point { -1, -1 }) {
		return false;
	}

	int horizontal = StairsDown.x - StairsDownPrevious.x + 10;
	int vertical = StairsDown.y - StairsDownPrevious.y + 10;

	if (horizontal < 0 || horizontal > MAXVIEWX)
		return false;
	if (vertical < 0 || vertical > MAXVIEWY)
		return false;
	return isVisible[vertical][horizontal];
}

int lengthPathToDlvl9 = 0;

bool IsGoodLevel()
{
	int maxDistance = 20;
	int lengthMaxPathToDlvl9 = 72;

	if (leveltype == DTYPE_CATACOMBS || leveltype == DTYPE_CATHEDRAL) {
		int cDistance = CalcStairsChebyshevDistance();
		StairsDownPrevious = StairsDown;

		if (cDistance != -1 && cDistance > maxDistance)
			return false;

		int stairsPath = PathLength();
		lengthPathToDlvl9 = lengthPathToDlvl9 + stairsPath;

		if (lengthPathToDlvl9 > lengthMaxPathToDlvl9)
			return false;
		if (stairsPath == 0 || stairsPath > maxDistance)
			return false;
	}else //(leveltype == DTYPE_CAVES || leveltype == DTYPE_HELL)
	{
		maxDistance = 15;

		bool isStairsVisibile = IsVisibleSpawn() || IsVisiblePrevious();
		StairsDownPrevious = StairsDown;
		
		if (currlevel != 9 && isStairsVisibile)
			return true;

		int cDistance = CalcStairsChebyshevDistance(); // CONDITIONALLY REWORK REQUIRED: WITH TELEPORT, DLVL 10+ DEPENDING ON DESTINATION VECTOR, MOVEMENT DISTANCE WILL BE SHORTER OR LONGER

		if (cDistance != -1 && cDistance > maxDistance)
			return false;

		int stairsPath = PathLength(); // CONDITIONALLY OBSOLETED: WITH TELEPORT, DLVL 10+ WILL NOT USE THIS PATHING. ONLY USED FOR PATHING ON DLVL9

		if (stairsPath == 0 || stairsPath > maxDistance)
			return false;
	}
	return true;
}

void createSpecificDungeon()
{
	uint32_t lseed = glSeedTbl[currlevel];
	if (leveltype == DTYPE_CATHEDRAL)
		CreateL5Dungeon(lseed, 0);
	else if (leveltype == DTYPE_CATACOMBS)
		CreateL2Dungeon(lseed, 0);
	else if (leveltype == DTYPE_CAVES)
		CreateL3Dungeon(lseed, 0);
	else if (leveltype == DTYPE_HELL)
		CreateL4Dungeon(lseed, 0);
}

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

	int seconds = time(NULL);
	for (uint32_t seed = startSeed; seed < startSeed + seedCount; seed++) {
		if (!quiet)
			std::cout << "processing seed " << seed << std::endl;
		int elapsed = time(NULL) - seconds;
		if (elapsed >= 1) {
			std::cerr << "processing seed " << seed << std::endl;
			seconds += elapsed;
		}

		lengthPathToDlvl9 = 0;
		seedSelection(seed);
		InitQuests();
		//if (quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
		//	if (verbose)
		//		std::cout << "Game Seed: " << sgGameInitInfo.dwSeed << " thrown out: Sign Quest" << std::endl;
		//	continue;
		//}
		//if (quests[Q_WARLORD]._qactive != QUEST_NOTAVAIL) {
		//	if (verbose)
		//		std::cout << "Game Seed: " << sgGameInitInfo.dwSeed << " thrown out: Warlord" << std::endl;
		//	continue;
		//}

		{
			currlevel = 9;

			InitLevelMonsters();
			SetRndSeed(glSeedTbl[currlevel]);
			GetLevelMTypes();
			bool lavaLordFound = false;
			for (int i = 0; i < nummtypes; i++)
				lavaLordFound |= Monsters[i].mtype == MT_WMAGMA;
			if (!lavaLordFound)
				continue;
			int themeSeed = sglGameSeed;

			whatleveltype();
			createSpecificDungeon();
			InitStairCordinates();

			if (CalcStairsChebyshevDistance() > 10)
				continue;

			SetRndSeed(themeSeed);
			InitThemes();

			SetRndSeed(glSeedTbl[currlevel]);
			HoldThemeRooms();
			GetRndSeed();
			InitMonsters();
			//GetRndSeed();
			//InitObjects();
			//InitItems();
			//CreateThemeRooms();

			int monsterItems = numitems;
			for (int i = 0; i < nummonsters; i++) {
				int mid = monstactive[i];
				if (monster[mid].MType->mtype != MT_WMAGMA)
					continue;
				SetRndSeed(monster[mid]._mRndSeed);
				SpawnItem(mid, monster[mid]._mx, monster[mid]._my, TRUE);
			}

			//int objectItems = numitems;
			//for (int i = 0; i < nobjects; i++) {
			//	int oid = objectactive[i];
			//	createItemsFromObject(oid);
			//}

			bool foundPuzzler = false;
			for (int i = 0; i < numitems; i++) {
				int ii = itemactive[i];
				if (item[ii]._iMagical == ITEM_QUALITY_UNIQUE && item[ii]._iUid == 60) {
					int horizontal = std::max(Spawn.x, item[ii]._ix) - std::min(Spawn.x, item[ii]._ix);
					int vertical = std::max(Spawn.y, item[ii]._iy) - std::min(Spawn.y, item[ii]._iy);
					int distance = std::max(horizontal, vertical);
					if (distance < 20)
						foundPuzzler = true;
				}
			}
			if (foundPuzzler)
				std::cout << "Game seed: " << seed << std::endl;
			continue;

			//if (!quiet) {
			//	std::cout << "Monster Count: " << nummonsters << std::endl;
			//	for (int i = 0; i < nummonsters; i++) {
			//		std::cout << "Monster " << i << ": " << monster[monstactive[i]].mName << " (" << monster[monstactive[i]]._mRndSeed << ")" << std::endl;
			//	}
			//	std::cout << std::endl;
			//	std::cout << "Object Count: " << nobjects << std::endl;
			//	for (int i = 0; i < nobjects; i++) {
			//		int oid = objectactive[i];
			//		char objstr[50];
			//		GetObjectStr(oid, objstr);
			//		std::cout << "Object " << i << ": " << objstr << " (" << object[oid]._oRndSeed << ")" << std::endl;
			//	}
			//	std::cout << std::endl;
			//	std::cout << "Item Count: " << numitems << std::endl;
			//	for (int i = 0; i < numitems; i++) {
			//		std::string prefix = "";
			//		if (i >= objectItems)
			//			prefix = "Object ";
			//		else if (i >= monsterItems)
			//			prefix = "Monster ";
			//		std::cout << prefix << "Item " << i << ": " << item[itemactive[i]]._iIName << " (" << item[itemactive[i]]._iSeed << ")" << std::endl;
			//	}
			//}
			if (exportLevels)
				ExportDun();
		}

		for (int level = 9; level < NUMLEVELS; level++) {
			currlevel = level;
			whatleveltype();
			createSpecificDungeon();
			InitStairCordinates();

			if (!IsGoodLevel()) {
				if (level > quality || verbose) {
					std::cout << "Game Seed: " << sgGameInitInfo.dwSeed << " quality: ";
					for (int p = 0; p < level - 1; p++) {
						std::cout << "+";
					}
					std::cout << " (" << (level - 1) << ")" << std::endl;
					break;
				}
				break;
			}
			if (!quiet)
				printAsciiLevel();
			if (exportLevels)
				ExportDun();
		}
	}

	return 0;
}
