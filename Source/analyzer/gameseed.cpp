#include "gameseed.h"

#include <iostream>

#include "../../types.h"

#include "../funkMapGen.h"
#include "../monster.h"

DungeonMode ScannerGameSeed::getDungeonMode()
{
	return DungeonMode::BreakOnSuccess;
}

bool ScannerGameSeed::skipLevel(int level)
{
	if (level != 9)
		return true;
		return false;

	InitDungeonMonsters();

	bool hasLavaLoards = false;
	for (int i = 0; i < nummtypes && !hasLavaLoards; i++) 
		hasLavaLoards = Monsters[i].mtype == MT_WMAGMA; // TODO fix me


	return !hasLavaLoards;
}

bool ScannerGameSeed::levelMatches(std::optional<uint32_t> levelSeed)
{
	if (levelSeed == std::nullopt)
		return false;

	if (levelSeed == *Config.target) {
		std::cout << sgGameInitInfo.dwSeed << std::endl;
		return true;
	}

	return false;
}
