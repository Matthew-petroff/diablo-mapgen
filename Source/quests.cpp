/**
 * @file quests.cpp
 *
 * Implementation of functionality for handling quests.
 */
#include "all.h"

int qtopline;
BOOL questlog;
BYTE *pQLogCel;
/** Contains the quests of the current game. */
QuestStruct quests[MAXQUESTS];
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;
int ReturnLvlX;
int ReturnLvlY;
int ReturnLvlT;
/** current frame # for the quest pentagram selector */
int questpentframe;
int ReturnLvl;

/** Contains the data related to each quest_id. */
QuestData questlist[MAXQUESTS] = {
	// clang-format off
	// _qdlvl,  _qdmultlvl, _qlvlt,          _qdtype,     _qdrnd,  _qslvl,  _qflags,       _qdmsg,        _qlstr
	{       5,         -1, DTYPE_NONE,      Q_ROCK,      100,      0,       QUEST_SINGLE,  TEXT_INFRA5,   "The Magic Rock"           },
	{       9,         -1, DTYPE_NONE,      Q_MUSHROOM,  100,      0,       QUEST_SINGLE,  TEXT_MUSH8,    "Black Mushroom"           },
	{       4,         -1, DTYPE_NONE,      Q_GARBUD,    100,      0,       QUEST_SINGLE,  TEXT_GARBUD1,  "Gharbad The Weak"         },
	{       8,         -1, DTYPE_NONE,      Q_ZHAR,      100,      0,       QUEST_SINGLE,  TEXT_ZHAR1,    "Zhar the Mad"             },
	{      14,         -1, DTYPE_NONE,      Q_VEIL,      100,      0,       QUEST_SINGLE,  TEXT_VEIL9,    "Lachdanan"                },
	{      15,         -1, DTYPE_NONE,      Q_DIABLO,    100,      0,       QUEST_ANY,     TEXT_VILE3,    "Diablo"                   },
	{       2,          2, DTYPE_NONE,      Q_BUTCHER,   100,      0,       QUEST_ANY,     TEXT_BUTCH9,   "The Butcher"              },
	{       4,         -1, DTYPE_NONE,      Q_LTBANNER,  100,      0,       QUEST_SINGLE,  TEXT_BANNER2,  "Ogden's Sign"             },
	{       7,         -1, DTYPE_NONE,      Q_BLIND,     100,      0,       QUEST_SINGLE,  TEXT_BLINDING, "Halls of the Blind"       },
	{       5,         -1, DTYPE_NONE,      Q_BLOOD,     100,      0,       QUEST_SINGLE,  TEXT_BLOODY,   "Valor"                    },
	{      10,         -1, DTYPE_NONE,      Q_ANVIL,     100,      0,       QUEST_SINGLE,  TEXT_ANVIL5,   "Anvil of Fury"            },
	{      13,         -1, DTYPE_NONE,      Q_WARLORD,   100,      0,       QUEST_SINGLE,  TEXT_BLOODWAR, "Warlord of Blood"         },
	{       3,          3, DTYPE_CATHEDRAL, Q_SKELKING,  100,      1,       QUEST_ANY,     TEXT_KING2,    "The Curse of King Leoric" },
	{       2,         -1, DTYPE_CAVES,     Q_PWATER,    100,      4,       QUEST_SINGLE,  TEXT_POISON3,  "Poisoned Water Supply"    },
	{       6,         -1, DTYPE_CATACOMBS, Q_SCHAMB,    100,      2,       QUEST_SINGLE,  TEXT_BONER,    "The Chamber of Bone"      },
	{      15,         15, DTYPE_CATHEDRAL, Q_BETRAYER,  100,      5,       QUEST_ANY,     TEXT_VILE1,    "Archbishop Lazarus"       },
#ifdef HELLFIRE
	{      17,         17, DTYPE_NONE,      Q_GRAVE,     100,      0,       QUEST_ANY,     TEXT_GRAVE7,   "Grave Matters"            },
	{      9,           9, DTYPE_NONE,      Q_FARMER,    100,      0,       QUEST_ANY,     TEXT_FARMER1,  "Farmer's Orchard"         },
	{      17,         -1, DTYPE_NONE,      Q_GIRL,      100,      0,       QUEST_SINGLE,  TEXT_GIRL2,    "Little Girl"              },
	{      19,         -1, DTYPE_NONE,      Q_TRADER,    100,      0,       QUEST_SINGLE,  TEXT_TRADER,   "Wandering Trader"         },
	{      17,         17, DTYPE_NONE,      Q_DEFILER,   100,      0,       QUEST_ANY,     TEXT_DEFILER1, "The Defiler"              },
	{      21,         21, DTYPE_NONE,      Q_NAKRUL,    100,      0,       QUEST_ANY,     TEXT_NAKRUL1,  "Na-Krul"                  },
	{      21,         -1, DTYPE_NONE,      Q_CORNSTN,   100,      0,       QUEST_SINGLE,  TEXT_CORNSTN,  "Cornerstone of the World" },
	{       9,          9, DTYPE_NONE,      Q_JERSEY,    100,      0,       QUEST_ANY,     TEXT_JERSEY4,  "The Jersey's Jersey"      },
#endif
	// clang-format on
};
/**
 * Specifies a delta in X-coordinates from the quest entrance for
 * which the hover text of the cursor will be visible.
 */
char questxoff[7] = { 0, -1, 0, -1, -2, -1, -2 };
/**
 * Specifies a delta in Y-coordinates from the quest entrance for
 * which the hover text of the cursor will be visible.
 */
char questyoff[7] = { 0, 0, -1, -1, -1, -2, -2 };
/**
 * A quest group containing the three quests the Butcher,
 * Ogden's Sign and Gharbad the Weak, which ensures that exactly
 * two of these three quests appear in any single player game.
 */
int QuestGroup1[3] = { Q_BUTCHER, Q_LTBANNER, Q_GARBUD };
/**
 * A quest group containing the three quests Halls of the Blind,
 * the Magic Rock and Valor, which ensures that exactly two of
 * these three quests appear in any single player game.
 */
int QuestGroup2[3] = { Q_BLIND, Q_ROCK, Q_BLOOD };
/**
 * A quest group containing the three quests Black Mushroom,
 * Zhar the Mad and Anvil of Fury, which ensures that exactly
 * two of these three quests appear in any single player game.
 */
int QuestGroup3[3] = { Q_MUSHROOM, Q_ZHAR, Q_ANVIL };
/**
 * A quest group containing the two quests Lachdanan and Warlord
 * of Blood, which ensures that exactly one of these two quests
 * appears in any single player game.
 */
int QuestGroup4[2] = { Q_VEIL, Q_WARLORD };

void InitQuests()
{
	int i, initiatedQuests;
	DWORD z;

	if (gbMaxPlayers == 1) {
		for (i = 0; i < MAXQUESTS; i++) {
			quests[i]._qactive = QUEST_NOTAVAIL;
		}
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (!(questlist[i]._qflags & QUEST_ANY)) {
				quests[i]._qactive = QUEST_NOTAVAIL;
			}
		}
	}

	questlog = FALSE;
	questpentframe = 1;
	WaterDone = 0;
	initiatedQuests = 0;

	for (z = 0; z < MAXQUESTS; z++) {
		if (gbMaxPlayers > 1 && !(questlist[z]._qflags & QUEST_ANY))
			continue;
		quests[z]._qtype = questlist[z]._qdtype;
		if (gbMaxPlayers > 1) {
			quests[z]._qlevel = questlist[z]._qdmultlvl;
			if (!delta_quest_inited(initiatedQuests)) {
				quests[z]._qactive = QUEST_INIT;
				quests[z]._qvar1 = 0;
				quests[z]._qlog = FALSE;
			}
			initiatedQuests++;
		} else {
			quests[z]._qactive = QUEST_INIT;
			quests[z]._qlevel = questlist[z]._qdlvl;
			quests[z]._qvar1 = 0;
			quests[z]._qlog = FALSE;
		}

		quests[z]._qslvl = questlist[z]._qslvl;
		quests[z]._qtx = 0;
		quests[z]._qty = 0;
		quests[z]._qidx = z;
		quests[z]._qlvltype = questlist[z]._qlvlt;
		quests[z]._qvar2 = 0;
		quests[z]._qmsg = questlist[z]._qdmsg;
	}

	if (gbMaxPlayers == 1) {
		SetRndSeed(glSeedTbl[15]);
		if (random_(0, 2) != 0)
			quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
		else
			quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

		quests[QuestGroup1[random_(0, sizeof(QuestGroup1) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup2[random_(0, sizeof(QuestGroup2) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup3[random_(0, sizeof(QuestGroup3) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup4[random_(0, sizeof(QuestGroup4) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	}
#ifdef _DEBUG
	if (questdebug != -1)
		quests[questdebug]._qactive = QUEST_ACTIVE;
#endif

#ifdef SPAWN
	for (z = 0; z < MAXQUESTS; z++) {
		quests[z]._qactive = QUEST_NOTAVAIL;
	}
#endif

	if (quests[Q_SKELKING]._qactive == QUEST_NOTAVAIL)
		quests[Q_SKELKING]._qvar2 = 2;
	if (quests[Q_ROCK]._qactive == QUEST_NOTAVAIL)
		quests[Q_ROCK]._qvar2 = 2;
	quests[Q_LTBANNER]._qvar1 = 1;
	if (gbMaxPlayers != 1)
		quests[Q_BETRAYER]._qvar1 = 2;
}

BOOL QuestStatus(int i)
{
	if (currlevel != quests[i]._qlevel)
		return FALSE;
	if (quests[i]._qactive == QUEST_NOTAVAIL)
		return FALSE;
	return TRUE;
}

void DrawButcher()
{
	int x, y;

	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;
	DRLG_RectTrans(x + 3, y + 3, x + 10, y + 10);
}

void DrawSkelKing(int q, int x, int y)
{
	quests[q]._qtx = 2 * x + 28;
	quests[q]._qty = 2 * y + 23;
}

void DrawWarLord(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L4Data\\Warlord2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 6;
			}
			dungeon[i][j] = v;
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawSChamber(int q, int x, int y)
{
	int i, j;
	int rw, rh;
	int xx, yy;
	BYTE *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L2Data\\Bonestr1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 3;
			}
			dungeon[i][j] = v;
			sp += 2;
		}
	}
	xx = 2 * x + 22;
	yy = 2 * y + 23;
	quests[q]._qtx = xx;
	quests[q]._qty = yy;
	mem_free_dbg(setp);
}

void DrawLTBanner(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				pdungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlind(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blind1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				pdungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlood(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DRLG_CheckQuests(int x, int y)
{
	int i;

	for (i = 0; i < MAXQUESTS; i++) {
		if (QuestStatus(i)) {
			switch (quests[i]._qtype) {
			case Q_BUTCHER:
				DrawButcher();
				break;
			case Q_LTBANNER:
				DrawLTBanner(x, y);
				break;
			case Q_BLIND:
				DrawBlind(x, y);
				break;
			case Q_BLOOD:
				DrawBlood(x, y);
				break;
			case Q_WARLORD:
				DrawWarLord(x, y);
				break;
			case Q_SKELKING:
				DrawSkelKing(i, x, y);
				break;
			case Q_SCHAMB:
				DrawSChamber(i, x, y);
				break;
			}
		}
	}
}

void SetReturnLvlPos()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		ReturnLvlX = quests[Q_SKELKING]._qtx + 1;
		ReturnLvlY = quests[Q_SKELKING]._qty;
		ReturnLvl = quests[Q_SKELKING]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_BONECHAMB:
		ReturnLvlX = quests[Q_SCHAMB]._qtx + 1;
		ReturnLvlY = quests[Q_SCHAMB]._qty;
		ReturnLvl = quests[Q_SCHAMB]._qlevel;
		ReturnLvlT = DTYPE_CATACOMBS;
		break;
	case SL_POISONWATER:
		ReturnLvlX = quests[Q_PWATER]._qtx;
		ReturnLvlY = quests[Q_PWATER]._qty + 1;
		ReturnLvl = quests[Q_PWATER]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlX = quests[Q_BETRAYER]._qtx + 1;
		ReturnLvlY = quests[Q_BETRAYER]._qty - 1;
		ReturnLvl = quests[Q_BETRAYER]._qlevel;
		ReturnLvlT = DTYPE_HELL;
		break;
	}
}

void GetReturnLvlPos()
{
	if (quests[Q_BETRAYER]._qactive == QUEST_DONE)
		quests[Q_BETRAYER]._qvar2 = 2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLvl;
	leveltype = ReturnLvlT;
}
