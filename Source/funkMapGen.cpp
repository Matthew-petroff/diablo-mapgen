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
#include "level.h"
#include "quests.h"
#include "trigs.h"

static int InitLevelType(int l)
{
	if (l == 0)
		return DTYPE_TOWN;
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

void InitStairCordinates()
{
	int l = currlevel;
	if (l == 0) { }
	if (l >= 1 && l <= 4)
		InitL1Triggers();
	if (l >= 5 && l <= 8)
		InitL2Triggers();
	if (l >= 9 && l <= 12)
		InitL3Triggers();
	if (l >= 13 && l <= 16)
		InitL4Triggers();
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
void seedSelection(int seed)
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

int getStairTeleportDistance()
{
	int upX = -1;
	int upY = -1;
	int downX = -1;
	int downY = -1;

	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i]._tmsg == WM_DIABNEXTLVL) {
			downX = trigs[i]._tx;
			downY = trigs[i]._ty;
		} else if (trigs[i]._tmsg == WM_DIABPREVLVL) {
			upX = trigs[i]._tx;
			upY = trigs[i]._ty;
		}
	}

	if (upX != -1 && upY != -1 && downX != -1 && downY != -1) {
		int H = std::max(upX, downX) - std::min(upX, downX);
		int V = std::max(upY, downY) - std::min(upY, downY);
		return std::max(H, V);
	}

	return -1;
}

void printAsciiLevel()
{
	for (int boby = 16; boby < MAXDUNY - 17; boby++) {
		for (int bobx = 16; bobx < MAXDUNX - 17; bobx++) {
			if (nSolidTable[dPiece[bobx][boby]])
				std::cout << "#";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}

	int distance = getStairTeleportDistance();
	if (distance != -1) {
		std::cout << "Stair teleport distance: " << distance << std::endl;
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
}

int main(int argc, char **argv)
{
	int startSeed = 0;
	int seedCount = 1;
	bool quiet = false;
	bool exportLevels = false;

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
			startSeed = std::stoi(argv[i + 1]);
		} else if (arg == "--count" && argc >= i + 1) {
			seedCount = std::stoi(argv[i + 1]);
		}
	}

	for (int seed = startSeed; seed < startSeed + seedCount; seed++) {
		if (!quiet)
			std::cout << "processing seed " << seed << std::endl;

		seedSelection(seed);
		InitQuests();

		for (int level = 1; level < NUMLEVELS; level++) {
			currlevel = level;
			whatleveltype();
			createSpecificDungeon();
			InitStairCordinates();

			if (!quiet)
				printAsciiLevel();
			if (exportLevels)
				ExportDun();
		}
	}

	return 0;
}
