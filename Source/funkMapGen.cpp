#include "funkMapGen.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "analyzer/gameseed.h"
#include "analyzer/path.h"
#include "analyzer/pattern.h"
#include "analyzer/puzzler.h"
#include "analyzer/quest.h"
#include "analyzer/scannerName.h"
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
	InitDungeonMonsters();

	InitThemes();
	SetRndSeed(glSeedTbl[currlevel]);
	HoldThemeRooms();
	GetRndSeed();

	InitMonsters();
	GetRndSeed();

	InitObjects();

	InitItems();
	CreateThemeRooms();
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

		if (mode != DungeonMode::NoContent && mode != DungeonMode::BreakOnFailureOrNoContent)
			CreateDungeonContent();

		if (currlevel == 15) {
			// Locate Lazarus warp point
			Point point = { quests[Q_BETRAYER]._qtx, quests[Q_BETRAYER]._qty };
			if (!nSolidTable[dPiece[point.x][point.y]])
				POI = point;
		}

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

std::vector<uint32_t> readFromFile()
{
	if (Config.seedFile.empty())
		return {};

	std::ifstream file(Config.seedFile);
	if (!file.is_open()) {
		std::cerr << "Unable to read seeds file: " << Config.seedFile << std::endl;
		exit(255);
	}

	if (!Config.quiet)
		std::cerr << "Loading seeds from: " << Config.seedFile << std::endl;

	std::vector<uint32_t> SeedsFromFile;
	std::string line;
	while (std::getline(file, line))
		SeedsFromFile.push_back(std::stoll(line.substr(0, line.find(' '))));

	Config.seedCount = std::min(Config.seedCount, uint32_t(SeedsFromFile.size() - Config.startSeed));

	if (file.is_open())
		file.close();

	return SeedsFromFile;
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

}

void InitDungeonMonsters()
{
	InitLevelMonsters();
	SetRndSeed(glSeedTbl[currlevel]);
	GetLevelMTypes();
}

int main(int argc, char **argv)
{
	Config = Configuration::ParseArguments(argc, argv);
	InitEngine();
	std::vector<uint32_t> SeedsFromFile = readFromFile();

	ProgressseedMicros = micros();
	for (uint32_t seedIndex = 0; seedIndex < Config.seedCount; seedIndex++) {
		uint32_t seed = seedIndex + Config.startSeed;
		if (!SeedsFromFile.empty()) {
			seed = SeedsFromFile[seed];
		}
		printProgress(seedIndex, seed);

		SetGameSeed(seed);
		if (scanner->skipSeed())
			continue;

		for (int level = 1; level < NUMLEVELS; level++) {
			if (scanner->skipLevel(level))
				continue;

			InitiateLevel(level);
			std::optional<uint32_t> levelSeed = CreateDungeon(scanner->getDungeonMode());
			if (!scanner->levelMatches(levelSeed))
				continue;

			if (Config.asciiLevels)
				printAsciiLevel();
			if (Config.exportLevels)
				ExportDun(seed);
		}
	}

	ShutDownEngine();

	return 0;
}
