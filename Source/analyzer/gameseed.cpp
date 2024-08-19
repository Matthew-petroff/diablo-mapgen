/**
 * @file analyzer/gameseed.cpp
 *
 * Implementation of scanner for finding the game seed that matches the target level seed.
 */

#include "gameseed.h"

#include <iostream>

#include "../../types.h"

#include "../funkMapGen.h"
#include "../monster.h"

DungeonMode ScannerGameSeed::getDungeonMode()
{
	return DungeonMode::BreakOnSuccess;
}

/*
 * @brief Skips the level if it is not level 9 or if it does not contain Lava Lords.
 */
bool ScannerGameSeed::skipLevel(int level)
{
	if (level != 9)
		return true;

	InitDungeonMonsters();

	bool hasLavaLords = false;
	for (int i = 0; i < nummtypes && !hasLavaLords; i++)
		hasLavaLords = Monsters[i].mtype == MT_WMAGMA;

	return !hasLavaLords;
}

/*
 * @brief Captures game seed if the level seed matches the target level seed.
 */
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
