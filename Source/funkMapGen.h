#pragma once

#include <string>

#include "engine.h"
#include "analyzer/scannerName.h"

struct Configuration {
	uint32_t startSeed = 0;
	uint32_t seedCount = 1;
	std::string seedFile;
	Scanners scanner = Scanners::None;
	bool quiet = false;
	bool asciiLevels = false;
	bool exportLevels = false;
	std::optional<uint32_t> target = std::nullopt;
	bool verbose = false;
};

extern Configuration Config;

class Scanner {
public:
	virtual void init() {};

	virtual DungeonMode getDungeonMode()
	{
		return DungeonMode::Full;
	};

	virtual bool skipSeed()
	{
		return false;
	};

	virtual bool skipLevel(int level)
	{
		return Config.target && level != *Config.target;
	};

	virtual bool levelMatches(std::optional<uint32_t> levelSeed)
	{
		return true;
	};

	virtual ~Scanner()
	{
	}
};

extern int MonsterItems;
extern int ObjectItems;

extern Point Spawn;
extern Point StairsDown;
extern Point POI;

extern char Path[MAX_PATH_LENGTH];

void InitDungeonMonsters();
