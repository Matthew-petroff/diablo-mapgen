#include "funkMapGen.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <vector>

#include "analyzer/gameseed.h"
#include "analyzer/path.h"
#include "analyzer/pattern.h"
#include "analyzer/puzzler.h"
#include "analyzer/quest.h"
#include "analyzer/stairs.h"
#include "analyzer/warp.h"
#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "drlg_l4.h"
#include "items.h"
#include "level.h"
#include "lighting.h"
#include "monster.h"
#include "objects.h"
#include "quests.h"
#include "themes.h"
#include "trigs.h"

int MonsterItems;
int ObjectItems;

Point Spawn = { -1, -1 };
Point StairsDown = { -1, -1 };
Point POI = { -1, -1 };

char Path[MAX_PATH_LENGTH];

Configuration Config;

namespace {

constexpr uint64_t ProgressInterval = 10 * 1000 * 1000;

BYTE previousLevelType = DTYPE_NONE;
Scanner *scanner;

void InitEngine()
{
	gnDifficulty = DIFF_NORMAL;
	leveltype = DTYPE_NONE;

	DRLG_PreLoadL2SP();
	DRLG_PreLoadDiabQuads();

	if (Config.scanner == Scanners::None) {
		scanner = new Scanner();
	} else if (Config.scanner == Scanners::Path) {
		scanner = new ScannerPath();
	} else if (Config.scanner == Scanners::Quest) {
		scanner = new ScannerQuest();
	} else if (Config.scanner == Scanners::Puzzler) {
		scanner = new ScannerPuzzler();
	} else if (Config.scanner == Scanners::Warp) {
		scanner = new ScannerWarp();
	} else if (Config.scanner == Scanners::Stairs) {
		scanner = new ScannerStairs();
	} else if (Config.scanner == Scanners::Pattern) {
		scanner = new ScannerPattern();
	} else if (Config.scanner == Scanners::GameSeed) {
		scanner = new ScannerGameSeed();
	}
}

void ShutDownEngine()
{
	DRLG_UnloadL2SP();
	DRLG_FreeDiabQuads();
	delete scanner;
}

void InitiateLevel(int level)
{
	POI = { -1, -1 };
	currlevel = level;

	oobread = false;
	oobwrite = false;

	if (level > 12)
		leveltype = DTYPE_HELL;
	else if (level > 8)
		leveltype = DTYPE_CAVES;
	else if (level > 4)
		leveltype = DTYPE_CATACOMBS;
	else if (level > 0)
		leveltype = DTYPE_CATHEDRAL;

	InitVision();

	if (leveltype == previousLevelType)
		return;
	previousLevelType = leveltype;

	LoadLvlGFX();
	FillSolidBlockTbls();
}

void InitTriggers()
{
	if (leveltype == DTYPE_CATHEDRAL)
		InitL1Triggers();
	else if (leveltype == DTYPE_CATACOMBS)
		InitL2Triggers();
	else if (leveltype == DTYPE_CAVES)
		InitL3Triggers();
	else if (leveltype == DTYPE_HELL)
		InitL4Triggers();
	Freeupstairs();
}

void FindStairCordinates()
{
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

void CreateDungeonContent()
{
}

std::optional<uint32_t> CreateDungeon(DungeonMode mode)
{
	uint32_t lseed = glSeedTbl[currlevel];
	std::optional<uint32_t> levelSeed = std::nullopt;
	if (leveltype == DTYPE_CATHEDRAL)
		levelSeed = CreateL5Dungeon(lseed, 0, mode);
	if (leveltype == DTYPE_CATACOMBS)
		levelSeed = CreateL2Dungeon(lseed, 0, mode);
	if (leveltype == DTYPE_CAVES)
		levelSeed = CreateL3Dungeon(lseed, 0, mode);
	if (leveltype == DTYPE_HELL)
		levelSeed = CreateL4Dungeon(lseed, 0, mode);

	if (mode == DungeonMode::Full || mode == DungeonMode::NoContent || mode == DungeonMode::BreakOnFailureOrNoContent) {
		InitTriggers();
		FindStairCordinates();
	}

	if (Config.verbose && oobwrite)
		std::cerr << "Game Seed: " << sgGameInitInfo.dwSeed << " OOB write detected" << std::endl;

	return levelSeed;
}

/**
 * @brief GET MAIN SEED, GET ALL MAP SEEDS
 * @return nothing, but updates RNG seeds list glSeedTbl[i]
 */
void SetGameSeed(uint32_t seed)
{
	sgGameInitInfo.dwSeed = seed;
	SetRndSeed(sgGameInitInfo.dwSeed);

	for (int i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = GetRndSeed();
	}

	InitQuests();
	memset(UniqueItemFlag, 0, sizeof(UniqueItemFlag));
}

std::vector<uint32_t> SeedsFromFile = {};

void readFromFile()
{
	if (Config.seedFile.empty())
		return;

	std::ifstream file(Config.seedFile);
	if (!file.is_open()) {
		std::cerr << "Unable to read seeds file: " << Config.seedFile << std::endl;
		exit(255);
	}

	if (!Config.quiet)
		std::cerr << "Loading seeds from: " << Config.seedFile << std::endl;

	std::string line;
	while (std::getline(file, line))
		SeedsFromFile.push_back(std::stoll(line.substr(0, line.find(' '))));

	Config.seedCount = std::min(Config.seedCount, uint32_t(SeedsFromFile.size() - Config.startSeed));

	if (file.is_open())
		file.close();
}

int ProgressseedMicros;
uint32_t ProgressseedIndex = 0;

void printProgress(uint32_t seedIndex, uint32_t seed)
{
	if (Config.verbose)
		std::cerr << "Processing: " << seed << std::endl;
	if (Config.quiet)
		return;

	int elapsed = micros() - ProgressseedMicros;
	if (elapsed < ProgressInterval)
		return;
	ProgressseedMicros += elapsed;

	uint64_t pct = 100 * (uint64_t)seedIndex / Config.seedCount;
	int speed = (seedIndex - ProgressseedIndex) / 10;
	int seconds = (Config.seedCount - seedIndex) / speed;
	ProgressseedIndex = seedIndex;

	int days = seconds / (24 * 3600);
	seconds %= (24 * 3600);
	int hours = seconds / 3600;
	seconds %= 3600;
	int minutes = seconds / 60;
	seconds %= 60;

	std::ostringstream oss;
	oss << "Progress: " << pct << "% eta: ";
	if (days > 0)
		oss << days << "d";
	if (hours > 0 || days > 0)
		oss << std::setw(2) << std::setfill('0') << hours << "h";
	if (minutes > 0 || hours > 0 || days > 0)
		oss << std::setw(2) << std::setfill('0') << minutes << "m";
	oss << std::setw(2) << std::setfill('0') << seconds << "s (" << speed << " seed/s)" << std::endl;

	std::cerr << oss.str();
}

void printHelp()
{
	std::cout << "--help         Print this message and exit" << std::endl;
	std::cout << "--ascii        Print ASCII version of levels" << std::endl;
	std::cout << "--export       Export levels as .dun files" << std::endl;
	std::cout << "--scanner <#>  How to analyze levels [default: none]" << std::endl;
	std::cout << "                   none: No analyzing" << std::endl;
	std::cout << "                   puzzler: Search for Naj's Puzzler on level 9" << std::endl;
	std::cout << "                   path: Estimate the time to complete the game" << std::endl;
	std::cout << "                   warp: Find seeds with a warp on level 15" << std::endl;
	std::cout << "                   stairs: Look for stairs with a very short distance to level 9" << std::endl;
	std::cout << "                   pattern: Search a set tile pattern" << std::endl;
	std::cout << "                   gameseed: Search for GameSeeds with LevelSeed" << std::endl;
	std::cout << "--start <#>    The seed to start from" << std::endl;
	std::cout << "--count <#>    The number of seeds to process" << std::endl;
	std::cout << "--seeds <#>    A file to read seeds from" << std::endl;
	std::cout << "--target <#>   The target for the current filter [default: 420]" << std::endl;
	std::cout << "--quiet        Do print status messages" << std::endl;
	std::cout << "--verbose      Print out details about seeds" << std::endl;
}

void ParseArguments(int argc, char **argv)
{
	bool fromFile = false;
	bool hasCount = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--help") {
			printHelp();
			exit(0);
		} else if (arg == "--quiet") {
			Config.quiet = true;
		} else if (arg == "--ascii") {
			Config.asciiLevels = true;
		} else if (arg == "--export") {
			Config.exportLevels = true;
		} else if (arg == "--scanner") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --scanner" << std::endl;
				exit(255);
			}
			std::string scanner = argv[i];
			if (scanner == "none") {
				Config.scanner = Scanners::None;
			} else if (scanner == "puzzler") {
				Config.scanner = Scanners::Puzzler;
			} else if (scanner == "path") {
				Config.scanner = Scanners::Path;
			} else if (scanner == "quest") {
				Config.scanner = Scanners::Quest;
			} else if (scanner == "warp") {
				Config.scanner = Scanners::Warp;
			} else if (scanner == "stairs") {
				Config.scanner = Scanners::Stairs;
			} else if (scanner == "pattern") {
				Config.scanner = Scanners::Pattern;
			} else if (scanner == "gameseed") {
				Config.scanner = Scanners::GameSeed;
			} else {
				std::cerr << "Unknown scanner: " << scanner << std::endl;
				exit(255);
			}
		} else if (arg == "--seeds") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing filename for --seeds" << std::endl;
				exit(255);
			}
			fromFile = true;
			Config.seedFile = argv[i];
		} else if (arg == "--start") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --start" << std::endl;
				exit(255);
			}
			Config.startSeed = std::stoll(argv[i]);
		} else if (arg == "--count") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --count" << std::endl;
				exit(255);
			}
			hasCount = true;
			Config.seedCount = std::stoll(argv[i]);
		} else if (arg == "--target") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --target" << std::endl;
				exit(255);
			}
			Config.target = std::stoll(argv[i]);
		} else if (arg == "--verbose") {
			Config.verbose = true;
		} else {
			std::cerr << "Unknown argument: " << arg << std::endl;
			exit(255);
		}
	}

	if (fromFile && !hasCount) {
		Config.seedCount = std::numeric_limits<uint32_t>::max();
	}
}

}

void InitDungeonMonsters()
{
}

int main(int argc, char **argv)
{
	ParseArguments(argc, argv);
	InitEngine();
	readFromFile();

	SetGameSeed(1230001659);
	InitiateLevel(9);
	CreateDungeon(DungeonMode::Full);

	SetRndSeed(glSeedTbl[currlevel]);
	GetLevelMTypes();
	InitThemes();
	HoldThemeRooms();

	ProgressseedMicros = micros();
	for (uint32_t seedIndex = 0; seedIndex < Config.seedCount; seedIndex++) {
		uint32_t seed = seedIndex + Config.startSeed;
		if (!SeedsFromFile.empty()) {
			seed = SeedsFromFile[seed];
		}
		printProgress(seedIndex, seed);

		memset(dMonster, 0, sizeof(dMonster));
		InitLevelMonsters();

		SetRndSeed(seed);
		GetRndSeed();
		InitMonsters();

		if (dMonster[29][21] || dMonster[30][21] || !dMonster[31][21] || !dMonster[32][21] || dMonster[33][21])
			continue;

		int mid = abs(dMonster[31][21]) - 1;
		auto &targetMonster = monster[mid];
		if (targetMonster.MType->mtype != MT_WMAGMA)
			continue;

		memset(dItem, 0, sizeof(dItem));
		memset(dObject, 0, sizeof(dItem));
		numitems = 0;
		InitObjects();
		InitItems();
		CreateThemeRooms();

		memset(UniqueItemFlag, 0, sizeof(UniqueItemFlag));

		SetRndSeed(targetMonster._mRndSeed);
		SpawnItem(mid, targetMonster._mx, targetMonster._my, TRUE);
		// std::cout << "Seed " << targetMonster.mName << ":" << targetMonster._mx << "x" << targetMonster._my << std::endl;
		bool found = false;
		if (numitems) {
			numitems--;
			auto &createdItem = item[itemactive[numitems]];
			//if (createdItem._itype == ITYPE_STAFF && createdItem._iUid == 60) {
			if (createdItem._itype == ITYPE_STAFF && createdItem._iUid == 60) {
				found = false;
				std::cout << "Seed " << seed << ":" << createdItem._iIName << " (" << createdItem._iSeed << ")" << std::endl;
				createdItem._iUid = 0;
			}
		}
		if (!found)
			continue;

		// auto &createdItem = item[itemactive[numitems]];
		// if (numitems) {
		//	numitems--;
		//	if (createdItem._itype == ITYPE_STAFF && createdItem._iUid == 60) {
		//		std::cout << "Seed " << seed << ":" << createdItem._iIName << " (" << createdItem._iSeed << ")" << std::endl;
		//		createdItem._iUid = 0;
		//	}
		//
		//	dItem[createdItem._ix][createdItem._iy] = 0;
		// }

		// SetRndSeed(seed);
		// CreateRndItem(40, 40, FALSE, TRUE, FALSE);
		// numitems--;
		// auto createdItem = item[itemactive[numitems]];
		// if (createdItem._itype == ITYPE_MISC && createdItem._iSpell ==  SPL_TOWN) {
		//	std::cout << "Seed " << seed << ":" << createdItem._iIName << " (" << createdItem._iSeed << ") " << std::endl;
		// }
		////if (createdItem._itype == ITYPE_RING && createdItem._iPLMana >> 6 == 19 && createdItem._iPLHP >> 6 == -22) {
		////	int mana = createdItem._iPLMana >> 6;
		////	int hp = createdItem._iPLHP >> 6;
		////	std::cout << "Seed " << seed << ":" << createdItem._iIName << " (" << createdItem._iSeed << ") " << "mana " << mana << " hp " << hp << std::endl;
		////}
		// dItem[createdItem._ix][createdItem._iy] = 0;

		std::cerr << "Forced seed " << seed << std::endl;
		ExportDun(seed);
	}

	ShutDownEngine();

	return 0;
}
