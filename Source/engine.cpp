#include <algorithm>
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

#include "engine.h"
#include "gendung.h"

PlayerStruct plr[MAX_PLRS];
DWORD glSeedTbl[NUMLEVELS];
_gamedata sgGameInitInfo;
int gnLevelTypeTbl[NUMLEVELS];
BOOL light4flag;

const int RndMult = 0x015A4E35;
const int RndInc = 1;
BYTE gbMaxPlayers = 1;
BOOL leveldebug = false;
bool zoomflag = false;

/** Seed value before the most recent call to SetRndSeed() */
int orgseed;
/** Current game seed */
int sglGameSeed;
/** Number of times the current seed has been fetched */
int SeedCount;

int questdebug = -1;

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
	return abs(sglGameSeed);
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
		std::cout << cwd << std::endl;
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
	free(pMegaTiles);
	//free(pLevelPieces);

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		pMegaTiles = LoadFileInMem("Levels\\L1Data\\L1.TIL", NULL);
		//pLevelPieces = LoadFileInMem("Levels\\L1Data\\L1.MIN", NULL);
		return;
	case DTYPE_CATACOMBS:
		pMegaTiles = LoadFileInMem("Levels\\L2Data\\L2.TIL", NULL);
		//pLevelPieces = LoadFileInMem("Levels\\L2Data\\L2.MIN", NULL);
		return;
	case DTYPE_CAVES:
		pMegaTiles = LoadFileInMem("Levels\\L3Data\\L3.TIL", NULL);
		//pLevelPieces = LoadFileInMem("Levels\\L3Data\\L3.MIN", NULL);
		return;
	case DTYPE_HELL:
		pMegaTiles = LoadFileInMem("Levels\\L4Data\\L4.TIL", NULL);
		//pLevelPieces = LoadFileInMem("Levels\\L4Data\\L4.MIN", NULL);
		return;
	default:
		app_fatal("LoadLvlGFX");
	}
}

void app_fatal(const char *dummystring)
{
	std::cout << dummystring << std::endl;
	exit(1);
}

void SetAutomapView(int nXPos, int nYPos)
{
}
