/**
 * @file analyzer/pattern.cpp
 *
 * Implementation of scanner for finding a level seed based on tile pattern.
 */

#include "pattern.h"

#include <cstdint>
#include <iostream>

#include "../../types.h"

#include "../engine.h"
#include "../gendung.h"
#include "../quests.h"

constexpr auto PatternX = 8;
constexpr auto PatternY = 9;

const uint8_t dungeonLevel1[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0,  0,  0,  0,  4,  0,  0 },
    {   0,  0, 66,204,  0,  0,  1,129,  0 },
    {   4, 63, 64, 65,  2,  0, 16,130,128 },
    {   1,129, 67, 68, 62, 57, 58,  0,135 },
    {   1,130,128,  0, 61, 59, 60,129,104 },
    {   0,  0,  0,  0,  0,  0,  0,130,128 },
    {   0,  0,  0,  0,  0,  0,140,140,140 },
    {   0,  0,  0,  0,  0,  0, 10,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel2[PatternX][PatternY] = {
	// clang-format off
    {   0,  0, 66,204,  0,  0,  0,  0,  0 },
    {   4, 63, 64, 65,108,  0,  0,  0,  0 },
    { 111,106, 67, 68, 62, 57, 58,  0,  0 },
    { 107,  0,  0,  0, 61, 59, 60,  0,  0 },
    {   1,129,  0,  0,  0,  0,  0,134,  0 },
    {   0,130,128,140,  0,  0,  0,  0,  0 },
    {   0,  0,  4,  2,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel3[PatternX][PatternY] = {
	// clang-format off
    {   0,  0, 66,204,  0,  0,  0,  0,  0 },
    {   4, 63, 64, 65,  2,  0,  0,  0, 16 },
    { 111,  0, 67, 68, 62, 57, 58,  0,  0 },
    {   1,129,106,  0, 61, 59, 60,  0,  0 },
    {   0,130,128,145,  0,  0,145,  0,  0 },
    {   0,104,129,142, 15,  0,142, 15,  0 },
    {   0,  0,130,128,  0,104,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel4[PatternX][PatternY] = {
	// clang-format off
    {   0,  0, 66,  0,  0,  0,  0,122,  0 },
    {   0, 63, 64, 65,  0,  0,  0,  0,  0 },
    {   0,  0, 67, 68, 62, 57, 58,  0,  0 },
    {   0,  0,  0,  0, 61, 59, 60,  0,  0 },
    { 107,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel5[PatternX][PatternY] = {
	// clang-format off
    {   1,  3,  3,  3,  3,  3,  0,  0,  0 },
    {  81,  3, 72, 77,  3, 48, 71,  0,  0 },
    {   1,  3, 76,  3,  3, 50, 78,  3,  0 },
    {   1,  0,114,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0, 13, 11, 11, 11, 11 },
    {   0,  0,  0,  0, 10, 12, 12, 12, 12 },
    {   0,  0,  0,  0,  0, 12, 12, 12, 12 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel6[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0,  0, 91,  0,  0,  0,  0 },
    {  77,  0, 48, 71, 89,  0,  0,  0,  0 },
    {   0,  0, 50, 78,  0, 90,  0,  0,  0 },
    {   0, 89,  0,120,122,  0,  0,  0,  0 },
    {   0,  0, 88,121,123,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel7[PatternX][PatternY] = {
	// clang-format off
    {   1,  0,  0,  0,  0,  0,  0,  0,  0 },
    {  82,  0, 72, 77,  0, 48, 71,  0,  0 },
    {   1,  0, 76,  0,  0, 50, 78,  0,  0 },
    {  80,  0,  0,  0,106,  0,107,  0,  0 },
    {   0,  0,  0,128,130,  0,  0,  0,  0 },
    {   0,  0,  0,129,131,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel8[PatternX][PatternY] = {
	// clang-format off
    {   1, 89,  0,  0,  0,  0,  0,  0,  0 },
    {  82,  0, 72, 77,  0, 48, 71,  0,  8 },
    {   1,  0, 76,  0,  0, 50, 78,  0, 80 },
    {  81,  0,  0,  0,  0,  0,  0, 48,  0 },
    {   0,  0, 87,  2,  0,  7,  0, 50,  9 },
    {   0,  0,  1,  0,  0,  4,  0,  0,  0 },
    {   0,  0,  0,  0,  0, 81,  0,  0,  0 },
    {   0,  0,  0,  0,  0, 83,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel9[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0,  0,  8,  0,  0,  0,  0 },
    {   0,  8,  8, 51, 50,  8,  8, 89,  0 },
    {   8,  8, 11, 48, 49,  3, 11, 88,  0 },
    {   8,  8, 47,106,  7, 13, 12,106,  0 },
    {   8,  8, 46,107,  7,106,108,106,  0 },
    {   0,  8,  1, 14,106,106,  5, 14,  0 },
    {   0,  0,  8,  1, 14,106,  4,  0,  0 },
    {   0,  0,  0,  0, 12,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel10[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0, 51, 50,  0, 92,  0,  0 },
    {   0,  0, 11, 48, 49,  3, 93,  3, 11 },
    {   0,  0, 23, 15, 20, 13, 12, 13, 12 },
    {   0,  0,  1, 14, 17,107,107,106,  0 },
    {   0,  0, 11, 12, 21, 16, 44,  0,  0 },
    {   0,  0, 47, 19, 20,106,  7, 29,  0 },
    {   0,  0, 46, 17, 17,108, 29, 34,  0 },
	// clang-format on
};

const uint8_t dungeonLevel11[PatternX][PatternY] = {
	// clang-format off
    {   0,  3,  8,  8,  8,  8,  0,  0,  0 },
    {   7, 13,  3, 51, 50,  8,  8,  0,  0 },
    {   7,  7, 13, 48, 49,  3,  8,  0,  0 },
    {   7,  7,  7,107,  7,  4,  8,  0,  0 },
    { 102, 14,  7,  7,  5,  6,  8,  0,  0 },
    {  97, 47,  7,  7,  4, 11,  0,  0,  0 },
    {   8, 46,106,106, 13, 12,  0,  0,  0 },
    {   8,130,134,136,146,136,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel12[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0,  8,  8,  8,  8, 47,107 },
    {   0, 11,  3, 51, 50,  8,  8, 46,  7 },
    {  87, 12, 13, 48, 49,  3,  8,130,136 },
    {   1, 14,106,106,  7,  4,  8,  1, 14 },
    {   0, 12,106,  7,  5,  6,  8,  8,  1 },
    {   0,  0,106,107, 13,  3,  8,  0,  0 },
    {   0,  0,  0,  0,  0,  4,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel13[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0, 50, 48, 10,  0,  0,  0 },
    {   0, 36, 38, 35, 47, 81, 49,  0,  6 },
    {   0, 37, 34, 33, 32,  6,  6,  6,  0 },
    {  12,  6, 97, 31,  0,  7, 51,  6, 44 },
    {   0, 50,  6,  6,  0,  0,  6,  6, 46 },
    {   0,  0,  0,  0,  0,  0,  0, 50,  6 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel14[PatternX][PatternY] = {
	// clang-format off
    {   0, 51,  6,  6,  0,  0,  0,  0,  0 },
    {   6,  0, 45, 41, 97,  0,  0,  0,  0 },
    {   6, 44, 43, 40,  6,  0,  0,  0,  0 },
    {   6, 46, 42, 39,  6,  0,  0,  0,  0 },
    {  49,  6,  6,  6,  6,  0,  0,  0,  0 },
    {   0, 36, 38, 35,  6,  0,  0,  0,  0 },
    {   0, 37, 34, 33, 32,  0,  0,  0,  0 },
    {   0, 50,  6, 31, 50,  0,  0,  0,  0 },
	// clang-format on
};

const uint8_t dungeonLevel15[PatternX][PatternY] = {
	// clang-format off
    {   0,  0, 36, 38, 35,  6, 51, 48,  0 },
    {   0,  0, 37, 34, 33, 32,  6, 47,  4 },
    {   0,  0, 49,  6, 31,  6,  6,  6,  0 },
    {   0,  0,  6, 49,  6,  6, 50,  6,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  0,  0,  0,  0,  0,  0,  0,  0 },
    {   0,  9,  2,  0,  0,  0, 70,  2,  0 },
    {   0,  0, 50,  6, 49, 95,  6, 47, 15 },
	// clang-format on
};

const uint8_t dungeonLevel16[PatternX][PatternY] = {
	// clang-format off
    {   0,  0,  0, 96,  6,  6,  0,119,117 },
    {   0, 36, 38, 35, 51,  6,  0,116,  9 },
    {   0, 37, 34, 33, 32,  6,  0,116,  0 },
    {   0, 96,  6, 31,  6,  6,  0,116,  0 },
    {   0,  6, 51,  6,  6,  6,  0,116,  0 },
    {   0,  0, 50, 51,  6,  6,  0,116,  0 },
    {   0,  0,  6, 97,  6,  6,  0,116,  0 },
    {   0,  0,  0,  0,  0,  0,  0,116,125 },
	// clang-format on
};

static bool UseSolidScanner(int level)
{
	return false;
}

static bool UseObjectScanner(int level)
{
	return false;
}

DungeonMode ScannerPattern::getDungeonMode()
{
	if (UseObjectScanner(currlevel))
		return DungeonMode::Full;
	if (UseSolidScanner(currlevel))
		return DungeonMode::BreakOnFailureOrNoContent;

	return DungeonMode::BreakOnFailure;
}

static void ForceSeeds(int level)
{
	// Cathedral
	glSeedTbl[1] = sgGameInitInfo.dwSeed; // Matches level seed 1691869883, dungeon seed 1342003104, game seed 1230144506 24 18:48
	quests[Q_BUTCHER]._qactive = QUEST_INIT;
	quests[Q_PWATER]._qactive = QUEST_INIT;
	glSeedTbl[2] = sgGameInitInfo.dwSeed;         // Matches level seed 3227087147, dungeon seed  755133500, game seed 1229972777 22 19:06
	quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL; // QUEST_INIT;
	glSeedTbl[3] = sgGameInitInfo.dwSeed;
	quests[Q_LTBANNER]._qactive = QUEST_NOTAVAIL;
	glSeedTbl[4] = sgGameInitInfo.dwSeed;

	// Catacombs
	quests[Q_BLOOD]._qactive = QUEST_INIT;
	glSeedTbl[5] = sgGameInitInfo.dwSeed; // Matches level seed 3155785492, dungeon seed 1293295771, game seed 1229975451 22 19:50
	quests[Q_SCHAMB]._qactive = QUEST_INIT;
	glSeedTbl[6] = sgGameInitInfo.dwSeed; // Matches level seed 417801337, dungeon seed 2062861350, game seed 1230145611 24 19:06
	quests[Q_BLIND]._qactive = QUEST_INIT;
	glSeedTbl[7] = sgGameInitInfo.dwSeed; // Matches level seed 3750712200, dungeon seed  894816128, game seed 1230145480 24 19:04
	glSeedTbl[8] = sgGameInitInfo.dwSeed; // Matches level seed 4191463473, dungeon seed 1560480383, game seed 1230145480 24 19:04

	// Caves
	glSeedTbl[9] = sgGameInitInfo.dwSeed; // Matches level seed 3916317768, dungeon seed  764458097, game seed 1230001659 23 03:07
	quests[Q_ANVIL]._qactive = QUEST_INIT;
	glSeedTbl[10] = sgGameInitInfo.dwSeed; // Matches level seed  618024415, dungeon seed 1375523899, game seed 1229973508 22 19:18
	glSeedTbl[11] = sgGameInitInfo.dwSeed; // Matches level seed 2376709555, dungeon seed  651290160, game seed 1229976092 22 20:01
	glSeedTbl[12] = sgGameInitInfo.dwSeed; // Matches level seed 1903656652, dungeon seed 2134483070, game seed 1230145177 24 18:59

	// Hell
	quests[Q_WARLORD]._qactive = QUEST_NOTAVAIL;
	glSeedTbl[13] = sgGameInitInfo.dwSeed; // Matches level seed 4250794344, dungeon seed  723487375, game seed 1229976431 22 20:07
	glSeedTbl[14] = sgGameInitInfo.dwSeed; // Matches level seed 1005627431, dungeon seed 2144005606, game seed 1229976755 22 20:12
	glSeedTbl[15] = sgGameInitInfo.dwSeed; // Matches level seed 2844841604, dungeon seed 1342549707, game seed 1230053637 23 17:33
	glSeedTbl[16] = sgGameInitInfo.dwSeed; // Matches level seed  277866386, dungeon seed 1993034597, game seed 1230059906 23 17:17
}

bool ScannerPattern::skipLevel(int level)
{
	bool skip = level == 3 || level == 4; // Pattern is still not correct
	if (Config.target)
		skip = level != *Config.target;

	if (!skip && !UseObjectScanner(level) && !UseSolidScanner(level))
		ForceSeeds(level); // @todo undiscombobulate the seeds and quests

	return skip;
}

constexpr auto SolidX = 26;
constexpr auto SolidY = 26;

/**
 * Data format of solid patterns
 *
 * 9: unknown
 * 0: probably walkable
 * 1: probably solid
 * 2: walkable
 * 3: solid
 */

const uint8_t Dlvl3Solid[SolidX][SolidY] = {
	// clang-format off
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9 },
	{ 9,9,9,3,3,3,3,2,2,3,3,3,3,3,3,3,3,3,3,3,2,9,9,9,9,9 },
	{ 9,9,9,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,9 },
	{ 9,9,9,3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,9,9,9 },
	{ 9,9,2,3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,9,9 },
	{ 9,3,9,3,0,9,9,0,0,0,0,0,0,3,2,3,0,9,9,0,0,0,0,0,9,9 },
	{ 9,9,2,3,0,9,9,0,0,0,0,0,0,0,0,0,0,9,9,0,0,0,0,0,9,9 },
	{ 9,9,9,3,0,9,9,9,9,0,0,0,0,0,0,0,0,9,9,9,9,0,0,9,9,1 },
	{ 9,9,9,1,0,9,9,9,9,0,0,0,0,0,0,0,0,9,9,9,9,0,9,9,9,1 },
	{ 9,9,9,9,0,0,0,9,9,0,0,3,2,0,0,0,0,3,2,9,9,9,9,9,9,3 },
	{ 9,9,9,9,9,0,0,9,9,0,0,2,2,0,0,0,0,2,2,9,9,9,9,9,9,2 },
	{ 9,9,9,9,9,9,0,9,9,9,9,0,0,0,0,0,0,0,0,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,0,0,0,0,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,3,2,9,9,9,9,3,2,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,2,2,9,9,9,9,2,2,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	// clang-format on
};

const uint8_t Dlvl4Solid[SolidX][SolidY] = {
	// clang-format off
	{ 9,9,9,9,9,9,9,9,1,1,1,1,1,1,9,9,9,9,1,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,1,1,1,1,1,1,9,9,9,9,1,0,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,1,1,1,1,1,1,1,1,1,1,1,1,1,0,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,1,3,3,3,3,1,1,1,1,1,1,1,3,2,0,0,9,9,9,9 },
	{ 9,9,9,9,3,3,3,3,0,0,3,3,3,3,3,3,3,3,3,2,0,0,9,9,9,9 },
	{ 9,9,9,9,3,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,9,9 },
	{ 9,9,1,1,3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,9,9 },
	{ 9,9,1,1,3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,9,9 },
	{ 9,9,1,1,3,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,9,9 },
	{ 9,0,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9 },
	{ 9,2,3,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9 },
	{ 9,0,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9 },
	{ 9,9,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,9 },
	{ 9,9,9,9,9,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,9,9,9,9 },
	{ 9,9,9,9,9,9,1,1,3,3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1 },
	{ 9,9,9,9,9,9,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	{ 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9 },
	// clang-format on
};

bool MatchesSolidPattern()
{
	if (StairsDown != Point { Spawn.x + 6, Spawn.y + 2 })
		return false;

	const uint8_t(*pattern)[SolidX][SolidY];
	int xOffset;
	int yoffset;
	if (currlevel == 3) {
		pattern = &Dlvl3Solid;
		xOffset = 11;
		yoffset = 5;
	} else if (currlevel == 4) {
		pattern = &Dlvl4Solid;
		xOffset = -3;
		yoffset = 18;
	}

	int misses = 0;
	for (int row = 0; row < SolidX; row++) {
		for (int column = 0; column < SolidY; column++) {
			if ((*pattern)[row][column] == 9)
				continue;
			int x = Spawn.x + row - xOffset;
			int y = Spawn.y + column - yoffset;

			if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY || nSolidTable[dPiece[y][x]] != ((*pattern)[row][column] % 2)) {
				misses++;
				if (misses > 4 || (*pattern)[row][column] > 1) {
					return false;
				}
			}
		}
	}

	std::cout << sgGameInitInfo.dwSeed << " possible game Seed for dlvl " << (int)currlevel << std::endl;

	return true;
}

static bool MatchesObjectPattern()
{
	if (currlevel == 3) {
		if (StairsDown != Point { Spawn.x + 6, Spawn.y + 2 })
			return false;
		if (!dObject[Spawn.x + 2][Spawn.y + 3] || dObject[Spawn.x + 2][Spawn.y + 3] != abs(dObject[Spawn.x + 2][Spawn.y + 2]))
			return false;
		if (!dObject[Spawn.x + 3][Spawn.y + 3])
			return false;
		return true;
	}
	if (currlevel == 4) {
		if (StairsDown != Point { Spawn.x + 6, Spawn.y + 2 })
			return false;
		if (!dObject[Spawn.x + 0][Spawn.y + 3] || dObject[Spawn.x + 0][Spawn.y + 3] != abs(dObject[Spawn.x + 0][Spawn.y + 2]))
			return false;
		if (!dObject[Spawn.x + 2][Spawn.y + 5] || dObject[Spawn.x + 2][Spawn.y + 5] != abs(dObject[Spawn.x + 2][Spawn.y + 4]))
			return false;
	}
	if (currlevel == 6) {
		if (StairsDown != Point { Spawn.x + 5, Spawn.y + 1 })
			return false;
		if (!dObject[Spawn.x + 1][Spawn.y + 3])
			return false;
		if (!dObject[Spawn.x + 2][Spawn.y + 3])
			return false;
		if (!dObject[Spawn.x + 1][Spawn.y + 4])
			return false;
		if (!dObject[Spawn.x + 2][Spawn.y + 4])
			return false;
		if (!dObject[Spawn.x + 3][Spawn.y + 5])
			return false;
	}
	if (currlevel == 7) {
		if (StairsDown != Point { Spawn.x + 5, Spawn.y + 1 })
			return false;
		if (!dObject[Spawn.x - 3][Spawn.y + 1])
			return false;
		if (!dObject[Spawn.x - 3][Spawn.y + 4])
			return false;
		if (!dObject[Spawn.x - 2][Spawn.y + 4])
			return false;
		if (!dObject[Spawn.x + 3][Spawn.y + 7])
			return false;
	}

	std::cout << sgGameInitInfo.dwSeed << " possible game Seed for dlvl " << (int)currlevel << std::endl;

	return true;
}

static bool MatchesTilePattern(std::optional<uint32_t> levelSeed)
{
	constexpr int xOffsetValues[] = { 3, 3, 3, 3, 3, 3, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3 };
	constexpr int stairTileValues[] = { 204, 65, 65, 65, 77, 71, 71, 71, 51, 51, 51, 51, 35, 41, 34, 35 };
	constexpr const uint8_t(*patterns[])[PatternX][PatternY] = {
		&dungeonLevel1, &dungeonLevel2, &dungeonLevel3, &dungeonLevel4, &dungeonLevel5,
		&dungeonLevel6, &dungeonLevel7, &dungeonLevel8, &dungeonLevel9, &dungeonLevel10,
		&dungeonLevel11, &dungeonLevel12, &dungeonLevel13, &dungeonLevel14,
		&dungeonLevel15, &dungeonLevel16
	};

	if (currlevel < 1 || currlevel > 16) {
		return false;
	}

	int stairTile = stairTileValues[currlevel - 1];
	int xOffset = xOffsetValues[currlevel - 1];
	const uint8_t(*pattern)[PatternX][PatternY] = patterns[currlevel - 1];

	bool foundStairs = false;
	int sx = -1;
	int sy = -1;
	for (int y = 0; y < DMAXX && !foundStairs; y++) {
		for (int x = 0; x < DMAXY; x++) {
			if (dungeon[x][y] == stairTile) {
				sx = x - xOffset;
				sy = y - 1;
				foundStairs = true;
				break;
			}
		}
	}

	if (!foundStairs) {
		if (Config.verbose)
			std::cerr << "PATTERN: Failed to locate stairs on level seed " << *levelSeed << std::endl;
		return false;
	}

	bool found = true;
	int misses = 0;
	for (int column = 0; column < PatternY; column++) {
		for (int row = 0; row < PatternX; row++) {
			if ((*pattern)[row][column] == 0)
				continue;
			int x = sx + column;
			int y = sy + row;
			if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY || dungeon[x][y] != (*pattern)[row][column]) {
				misses++;
				if (misses > 0) {
					found = false;
					break;
				}
			}
		}
		if (!found)
			return false;
	}

	std::cout << "PATTERN: Dungeon level " << (int)currlevel << "  seed: " << *levelSeed << std::endl;

	return true;
}

bool ScannerPattern::levelMatches(std::optional<uint32_t> levelSeed)
{
	if (levelSeed == std::nullopt)
		return false;
	if (UseObjectScanner(currlevel))
		return MatchesObjectPattern();
	if (UseSolidScanner(currlevel))
		return MatchesSolidPattern();
	return MatchesTilePattern(levelSeed);
}
