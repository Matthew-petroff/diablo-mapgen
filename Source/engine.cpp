#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <malloc.h>
#include <stdio.h>
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "asset/globalcache.h"
#include "engine.h"
#include "gendung.h"

PlayerStruct plr[MAX_PLRS];
DWORD glSeedTbl[NUMLEVELS];
_gamedata sgGameInitInfo;
BOOL light4flag;

const uint32_t RndMult = 0x015A4E35;
const uint32_t RndInc = 1;
BYTE gbMaxPlayers = 1;
BOOL leveldebug = false;
bool zoomflag = false;

/** Seed value before the most recent call to SetRndSeed() */
int orgseed;
/** Current game seed */
uint32_t sglGameSeed;
/** Number of times the current seed has been fetched */
int SeedCount;

int questdebug = -1;

uint64_t micros()
{
	auto now = std::chrono::steady_clock::now();
	auto since_epoch = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::microseconds>(since_epoch).count();
}

BOOL delta_quest_inited(int i)
{
	return false;
}

/**
 * @brief Set the RNG seed
 * @param s RNG seed
 */
void SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int GetRndSeed()
{
	SeedCount++;
	sglGameSeed = RndMult * sglGameSeed + RndInc;
	int32_t seed = (int32_t)sglGameSeed;
	return seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : std::abs(seed);
}

int GetRndState()
{
	return sglGameSeed;
}

int random_(BYTE idx, int v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (GetRndSeed() >> 16) % v;
	return GetRndSeed() % v;
}

BYTE *DiabloAllocPtr(DWORD dwBytes)
{
	return (BYTE *)malloc(dwBytes);
}

/**
 * @brief Multithreaded safe memfree
 * @param p Memory pointer to free
 */
void mem_free_dbg(void *p)
{
	free(p);
}

/**
 * @brief Load a file into a buffer
 * @param pszName Path of file
 * @param pdwFileLen Will be set to file size if non-NULL
 * @return Buffer with content of file
 */
BYTE *LoadFileInMem(std::string pszName, DWORD *pdwFileLen)
{
	BYTE *buf;
	int fileLen;

#ifndef WIN32
	// Convert to lowercase
	std::transform(pszName.begin(), pszName.end(), pszName.begin(), [](unsigned char c) { return std::tolower(c); });

	// Replace backslashes with forward slashes
	std::replace(pszName.begin(), pszName.end(), '\\', '/');
#endif

	FILE *filestream = fopen(pszName.c_str(), "r+b");

	if (!filestream) {
		char cwd[MAX_PATH];
		cwd[0] = '\0';
#ifdef _WIN32
		GetCurrentDirectoryA(sizeof(cwd), cwd);
#else
		getcwd(cwd, sizeof(cwd));
#endif
		std::cerr << cwd << std::endl;
		app_fatal("FILE NOT FOUND");
	}
	fseek(filestream, 0, SEEK_END); // seek to end of file
	fileLen = ftell(filestream);    // get current file pointer
	fseek(filestream, 0, SEEK_SET); // seek back to beginning of file

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		app_fatal("Zero length SFILE:\n%s");

	buf = (BYTE *)DiabloAllocPtr(fileLen);

	int read = fread(buf, 1, fileLen, filestream);

	fclose(filestream);

	return buf;
}

void LoadLvlGFX()
{
	auto& levelsCache = asset::GlobalCache::Get().diabdat.levels;
	pMegaTiles = nullptr;
	pLevelPieces = nullptr;

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		pMegaTiles = levelsCache.l1data.l1_til.GetData();
		pLevelPieces = levelsCache.l1data.l1_min.GetData();
		return;
	case DTYPE_CATACOMBS:
		pMegaTiles = levelsCache.l2data.l2_til.GetData();
		pLevelPieces = levelsCache.l2data.l2_min.GetData();
		return;
	case DTYPE_CAVES:
		pMegaTiles = levelsCache.l3data.l3_til.GetData();
		pLevelPieces = levelsCache.l3data.l3_min.GetData();
		return;
	case DTYPE_HELL:
		pMegaTiles = levelsCache.l4data.l4_til.GetData();
		pLevelPieces = levelsCache.l4data.l4_min.GetData();
		return;
	default:
		app_fatal("LoadLvlGFX");
	}
}

void app_fatal(const char *dummystring)
{
	std::cerr << dummystring << std::endl;
	exit(1);
}

void SetAutomapView(int nXPos, int nYPos)
{
}

bool oobread = false;
bool oobwrite = false;
