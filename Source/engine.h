#pragma once
#include "../types.h"

#include <string>
#define assert_fail(exp) ((void)(exp))

extern BYTE gbMaxPlayers;
extern BOOL leveldebug;
extern BOOL light4flag;
extern DWORD glSeedTbl[NUMLEVELS];
extern _gamedata sgGameInitInfo;
extern int gnLevelTypeTbl[NUMLEVELS];
extern int gnDifficulty;
extern PlayerStruct plr[MAX_PLRS];
extern bool zoomflag;
extern int questdebug;

BOOL delta_quest_inited(int i);
int random_(BYTE idx, int v);
[[noreturn]] void app_fatal(const char *dummystring);
void SetAutomapView(int nXPos, int nYPos);
void SetRndSeed(int s);
int GetRndSeed();

BYTE *DiabloAllocPtr(DWORD dwBytes);
void mem_free_dbg(void *p);
BYTE *LoadFileInMem(std::string pszName, DWORD *pdwFileLen);
void LoadLvlGFX();

void SetMapObjects(BYTE *pMap, int startx, int starty);

void SetMapMonsters(BYTE *pMap, int startx, int starty);
