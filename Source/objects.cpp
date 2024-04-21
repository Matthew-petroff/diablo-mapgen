/**
 * @file objects.cpp
 *
 * Implementation of object functionality, interaction, spawning, loading, etc.
 */
#include "all.h"

#include "asset/globalcache.h"

int trapid;
int trapdir;
BYTE *pObjCels[40];
char ObjFileList[40];
int objectactive[MAXOBJECTS];
/** Specifies the number of active objects. */
int nobjects;
int leverid;
int objectavail[MAXOBJECTS];
ObjectStruct object[MAXOBJECTS];
BOOL InitObjFlag;
int numobjfiles;
#ifdef HELLFIRE
int dword_6DE0E0;
#endif

/** Specifies the X-coordinate delta between barrels. */
int bxadd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
/** Specifies the Y-coordinate delta between barrels. */
int byadd[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
/** Maps from shrine_id to shrine name. */
const char *const shrinestrs[NUM_SHRINETYPE] = {
	"Mysterious",
	"Hidden",
	"Gloomy",
	"Weird",
	"Magical",
	"Stone",
	"Religious",
	"Enchanted",
	"Thaumaturgic",
	"Fascinating",
	"Cryptic",
	"Magical",
	"Eldritch",
	"Eerie",
	"Divine",
	"Holy",
	"Sacred",
	"Spiritual",
	"Spooky",
	"Abandoned",
	"Creepy",
	"Quiet",
	"Secluded",
	"Ornate",
	"Glimmering",
	"Tainted",
#ifdef HELLFIRE
	"Oily",
	"Glowing",
	"Mendicant's",
	"Sparkling",
	"Town",
	"Shimmering",
	"Solar",
	"Murphy's",
#endif
};
/** Specifies the minimum dungeon level on which each shrine will appear. */
char shrinemin[NUM_SHRINETYPE] = {
	1, // Mysterious
	1, // Hidden
	1, // Gloomy
	1, // Weird
	1, // Magical
	1, // Stone
	1, // Religious
	1, // Enchanted
	1, // Thaumaturgic
	1, // Fascinating
	1, // Cryptic
	1, // Magical
	1, // Eldritch
	1, // Eerie
	1, // Divine
	1, // Holy
	1, // Sacred
	1, // Spiritual
	1, // Spooky
	1, // Abandoned
	1, // Creepy
	1, // Quiet
	1, // Secluded
	1, // Ornate
	1, // Glimmering
	1, // Tainted
#ifdef HELLFIRE
	1, // Oily
	1, // Glowing
	1, // Mendicant's
	1, // Sparkling
	1, // Town
	1, // Shimmering
	1, // Solar,
	1, // Murphy's
#endif
};
/** Specifies the maximum dungeon level on which each shrine will appear. */
char shrinemax[NUM_SHRINETYPE] = {
	MAX_LVLS, // Mysterious
	MAX_LVLS, // Hidden
	MAX_LVLS, // Gloomy
	MAX_LVLS, // Weird
	MAX_LVLS, // Magical
	MAX_LVLS, // Stone
	MAX_LVLS, // Religious
	8,        // Enchanted
	MAX_LVLS, // Thaumaturgic
	MAX_LVLS, // Fascinating
	MAX_LVLS, // Cryptic
	MAX_LVLS, // Magical
	MAX_LVLS, // Eldritch
	MAX_LVLS, // Eerie
	MAX_LVLS, // Divine
	MAX_LVLS, // Holy
	MAX_LVLS, // Sacred
	MAX_LVLS, // Spiritual
	MAX_LVLS, // Spooky
	MAX_LVLS, // Abandoned
	MAX_LVLS, // Creepy
	MAX_LVLS, // Quiet
	MAX_LVLS, // Secluded
	MAX_LVLS, // Ornate
	MAX_LVLS, // Glimmering
	MAX_LVLS, // Tainted
#ifdef HELLFIRE
	MAX_LVLS, // Oily
	MAX_LVLS, // Glowing
	MAX_LVLS, // Mendicant's
	MAX_LVLS, // Sparkling
	MAX_LVLS, // Town
	MAX_LVLS, // Shimmering
	MAX_LVLS, // Solar,
	MAX_LVLS, // Murphy's
#endif
};
/**
 * Specifies the game type for which each shrine may appear.
 * SHRINETYPE_ANY - 0 - sp & mp
 * SHRINETYPE_SINGLE - 1 - sp only
 * SHRINETYPE_MULTI - 2 - mp only
 */

BYTE shrineavail[NUM_SHRINETYPE] = {
	SHRINETYPE_ANY,    // SHRINE_MYSTERIOUS
	SHRINETYPE_ANY,    // SHRINE_HIDDEN
	SHRINETYPE_SINGLE, // SHRINE_GLOOMY
	SHRINETYPE_SINGLE, // SHRINE_WEIRD
	SHRINETYPE_ANY,    // SHRINE_MAGICAL
	SHRINETYPE_ANY,    // SHRINE_STONE
	SHRINETYPE_ANY,    // SHRINE_RELIGIOUS
	SHRINETYPE_ANY,    // SHRINE_ENCHANTED
	SHRINETYPE_SINGLE, // SHRINE_THAUMATURGIC
	SHRINETYPE_ANY,    // SHRINE_FASCINATING
	SHRINETYPE_ANY,    // SHRINE_CRYPTIC
	SHRINETYPE_ANY,    // SHRINE_MAGICAL2
	SHRINETYPE_ANY,    // SHRINE_ELDRITCH
	SHRINETYPE_ANY,    // SHRINE_EERIE
	SHRINETYPE_ANY,    // SHRINE_DIVINE
	SHRINETYPE_ANY,    // SHRINE_HOLY
	SHRINETYPE_ANY,    // SHRINE_SACRED
	SHRINETYPE_ANY,    // SHRINE_SPIRITUAL
	SHRINETYPE_MULTI,  // SHRINE_SPOOKY
	SHRINETYPE_ANY,    // SHRINE_ABANDONED
	SHRINETYPE_ANY,    // SHRINE_CREEPY
	SHRINETYPE_ANY,    // SHRINE_QUIET
	SHRINETYPE_ANY,    // SHRINE_SECLUDED
	SHRINETYPE_ANY,    // SHRINE_ORNATE
	SHRINETYPE_ANY,    // SHRINE_GLIMMERING
	SHRINETYPE_MULTI,  // SHRINE_TAINTED
#ifdef HELLFIRE
	SHRINETYPE_ANY,    // SHRINE_OILY
	SHRINETYPE_ANY,    // SHRINE_GLOWING
	SHRINETYPE_ANY,    // SHRINE_MENDICANT
	SHRINETYPE_ANY,    // SHRINE_SPARKLING
	SHRINETYPE_ANY,    // SHRINE_TOWN
	SHRINETYPE_ANY,    // SHRINE_SHIMMERING
	SHRINETYPE_SINGLE, // SHRINE_SOLAR
	SHRINETYPE_ANY,    // SHRINE_MURPHYS
#endif
};
/** Maps from book_id to book name. */
const char *const StoryBookName[] = {
	"The Great Conflict",
	"The Wages of Sin are War",
	"The Tale of the Horadrim",
	"The Dark Exile",
	"The Sin War",
	"The Binding of the Three",
	"The Realms Beyond",
	"Tale of the Three",
	"The Black King",
#ifdef HELLFIRE
	"Journal: The Ensorcellment",
	"Journal: The Meeting",
	"Journal: The Tirade",
	"Journal: His Power Grows",
	"Journal: NA-KRUL",
	"Journal: The End",
	"A Spellbook",
#endif
};
/** Specifies the speech IDs of each dungeon type narrator book, for each player class. */
int StoryText[3][3] = {
	{ TEXT_BOOK11, TEXT_BOOK12, TEXT_BOOK13 },
	{ TEXT_BOOK21, TEXT_BOOK22, TEXT_BOOK23 },
	{ TEXT_BOOK31, TEXT_BOOK32, TEXT_BOOK33 }
};

DIABOOL RndLocOk(int xp, int yp)
{
	if (dMonster[xp][yp] != 0)
		return FALSE;
	if (dPlayer[xp][yp] != 0)
		return FALSE;
	if (dObject[xp][yp] != 0)
		return FALSE;
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;
	if (nSolidTable[dPiece[xp][yp]])
		return FALSE;
	if (leveltype != DTYPE_CATHEDRAL || dPiece[xp][yp] <= 126 || dPiece[xp][yp] >= 144)
		return TRUE;
	return FALSE;
}

static DIABOOL WallTrapLocOkK(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;

	if (nTrapTable[dPiece[xp][yp]] != FALSE)
		return TRUE;
	else
		return FALSE;
}

void InitRndLocObj(int min, int max, int objtype)
{
	int i, xp, yp, numobjs;

	numobjs = random_(139, max - min) + min;

	for (i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
			if (RndLocOk(xp - 1, yp - 1)
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, xp, yp);
				break;
			}
		}
	}
}

void InitRndLocBigObj(int min, int max, int objtype)
{
	int i, xp, yp, numobjs;

	numobjs = random_(140, max - min) + min;
	for (i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(140, 80) + 16;
			yp = random_(140, 80) + 16;
			if (RndLocOk(xp - 1, yp - 2)
			    && RndLocOk(xp, yp - 2)
			    && RndLocOk(xp + 1, yp - 2)
			    && RndLocOk(xp - 1, yp - 1)
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, xp, yp);
				break;
			}
		}
	}
}

void InitRndLocObj5x5(int min, int max, int objtype)
{
	DIABOOL exit;
	int xp, yp, numobjs, i, cnt, m, n;

	numobjs = min + random_(139, max - min);
	for (i = 0; i < numobjs; i++) {
		cnt = 0;
		exit = FALSE;
		while (!exit) {
			exit = TRUE;
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
			for (n = -2; n <= 2; n++) {
				for (m = -2; m <= 2; m++) {
					if (!RndLocOk(xp + m, yp + n))
						exit = FALSE;
				}
			}
			if (!exit) {
				cnt++;
				if (cnt > 20000)
					return;
			}
		}
		AddObject(objtype, xp, yp);
	}
}

void ClrAllObjects()
{
	int i;

#ifdef HELLFIRE
	memset(object, 0, sizeof(object));
#else
	for (i = 0; i < MAXOBJECTS; i++) {
		object[i]._ox = 0;
		object[i]._oy = 0;
		object[i]._oAnimData = 0;
		object[i]._oAnimDelay = 0;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = 0;
		object[i]._oAnimFrame = 0;
		object[i]._oDelFlag = FALSE;
		object[i]._oVar1 = 0;
		object[i]._oVar2 = 0;
		object[i]._oVar3 = 0;
		object[i]._oVar4 = 0;
	}
#endif
	nobjects = 0;
	for (i = 0; i < MAXOBJECTS; i++) {
		objectavail[i] = i;
#ifndef HELLFIRE
		objectactive[i] = 0;
#endif
	}
#ifdef HELLFIRE
	memset(objectactive, 0, sizeof(objectactive));
#endif
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}

void AddTortures()
{
	int ox, oy;

	for (oy = 0; oy < MAXDUNY; oy++) {
		for (ox = 0; ox < MAXDUNX; ox++) {
			if (dPiece[ox][oy] == 367) {
				AddObject(OBJ_TORTURE1, ox, oy + 1);
				AddObject(OBJ_TORTURE3, ox + 2, oy - 1);
				AddObject(OBJ_TORTURE2, ox, oy + 3);
				AddObject(OBJ_TORTURE4, ox + 4, oy - 1);
				AddObject(OBJ_TORTURE5, ox, oy + 5);
				AddObject(OBJ_TNUDEM1, ox + 1, oy + 3);
				AddObject(OBJ_TNUDEM2, ox + 4, oy + 5);
				AddObject(OBJ_TNUDEM3, ox + 2, oy);
				AddObject(OBJ_TNUDEM4, ox + 3, oy + 2);
				AddObject(OBJ_TNUDEW1, ox + 2, oy + 4);
				AddObject(OBJ_TNUDEW2, ox + 2, oy + 1);
				AddObject(OBJ_TNUDEW3, ox + 4, oy + 2);
			}
		}
	}
}
void AddCandles()
{
	int tx, ty;

	tx = quests[Q_PWATER]._qtx;
	ty = quests[Q_PWATER]._qty;
	AddObject(OBJ_STORYCANDLE, tx - 2, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx + 3, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx - 1, ty + 2);
	AddObject(OBJ_STORYCANDLE, tx + 2, ty + 2);
}

void AddBookLever(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2, int msg)
{
	DIABOOL exit;
	int xp, yp, ob, cnt, m, n;

	cnt = 0;
	exit = FALSE;
	while (!exit) {
		exit = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (n = -2; n <= 2; n++) {
			for (m = -2; m <= 2; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = FALSE;
			}
		}
		if (!exit) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}

	if (QuestStatus(Q_BLIND))
		AddObject(OBJ_BLINDBOOK, xp, yp);
	if (QuestStatus(Q_WARLORD))
		AddObject(OBJ_STEELTOME, xp, yp);
	if (QuestStatus(Q_BLOOD)) {
		xp = 2 * setpc_x + 25;
		yp = 2 * setpc_y + 40;
		AddObject(OBJ_BLOODBOOK, xp, yp);
	}
	ob = dObject[xp][yp] - 1;
	SetObjMapRange(ob, x1, y1, x2, y2, leverid);
	SetBookMsg(ob, msg);
	leverid++;
	object[ob]._oVar6 = object[ob]._oAnimFrame + 1;
}

void InitRndBarrels()
{
	int numobjs; // number of groups of barrels to generate
	int xp, yp;
	_object_id o;
	DIABOOL found;
	int p; // regulates chance to stop placing barrels in current group
	int dir;
	int t; // number of tries of placing next barrel in current group
	int c; // number of barrels in current group
	int i;

	numobjs = random_(143, 5) + 3;
	for (i = 0; i < numobjs; i++) {
		do {
			xp = random_(143, 80) + 16;
			yp = random_(143, 80) + 16;
		} while (!RndLocOk(xp, yp));
		o = (random_(143, 4) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
		AddObject(o, xp, yp);
		found = TRUE;
		p = 0;
		c = 1;
		while (random_(143, p) == 0 && found) {
			t = 0;
			found = FALSE;
			while (TRUE) {
				if (t >= 3)
					break;
				dir = random_(143, 8);
				xp += bxadd[dir];
				yp += byadd[dir];
				found = RndLocOk(xp, yp);
				t++;
				if (found)
					break;
			}
			if (found) {
				o = (random_(143, 5) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
				AddObject(o, xp, yp);
				c++;
			}
			p = c >> 1;
		}
	}
}

void AddL1Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 270)
				AddObject(OBJ_L1LIGHT, i, j);
			if (pn == 44 || pn == 51 || pn == 214)
				AddObject(OBJ_L1LDOOR, i, j);
			if (pn == 46 || pn == 56)
				AddObject(OBJ_L1RDOOR, i, j);
		}
	}
}

#ifdef HELLFIRE
void add_crypt_objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 77)
				AddObject(OBJ_L1LDOOR, i, j);
			if (pn == 80)
				AddObject(OBJ_L1RDOOR, i, j);
		}
	}
}

#endif
void AddL2Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 13 || pn == 541)
				AddObject(OBJ_L2LDOOR, i, j);
			if (pn == 17 || pn == 542)
				AddObject(OBJ_L2RDOOR, i, j);
		}
	}
}

void AddL3Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 531)
				AddObject(OBJ_L3LDOOR, i, j);
			if (pn == 534)
				AddObject(OBJ_L3RDOOR, i, j);
		}
	}
}

DIABOOL TorchLocOK(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;
	return TRUE;
}

void AddL2Torches()
{
	int i, j, pn;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (!TorchLocOK(i, j))
				continue;

			pn = dPiece[i][j];
			if (pn == 1 && random_(145, 3) == 0)
				AddObject(OBJ_TORCHL2, i, j);

			if (pn == 5 && random_(145, 3) == 0)
				AddObject(OBJ_TORCHR2, i, j);

			if (pn == 37 && random_(145, 10) == 0 && dObject[i - 1][j] == 0)
				AddObject(OBJ_TORCHL, i - 1, j);

			if (pn == 41 && random_(145, 10) == 0 && dObject[i][j - 1] == 0)
				AddObject(OBJ_TORCHR, i, j - 1);
		}
	}
}

void AddObjTraps()
{
	char oi_trap, oi;
	int i, j;
	int xp, yp;
	int rndv;

	if (currlevel == 1)
		rndv = 10;
	if (currlevel >= 2)
		rndv = 15;
	if (currlevel >= 5)
		rndv = 20;
	if (currlevel >= 7)
		rndv = 25;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dObject[i][j] <= 0 || random_(144, 100) >= rndv)
				continue;

			oi = dObject[i][j] - 1;
			if (!AllObjects[object[oi]._otype].oTrapFlag)
				continue;

			if (random_(144, 2) == 0) {
				xp = i - 1;
				while (!nSolidTable[GetdPiece(xp, j)])
					xp--;

				if (!WallTrapLocOkK(xp, j) || i - xp <= 1)
					continue;

				AddObject(OBJ_TRAPL, xp, j);
				oi_trap = dObject[xp][j] - 1;
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = TRUE;
			} else {
				yp = j - 1;
				while (!nSolidTable[dPiece[i][yp]])
					yp--;

				if (!WallTrapLocOkK(i, yp) || j - yp <= 1)
					continue;

				AddObject(OBJ_TRAPR, i, yp);
				oi_trap = dObject[i][yp] - 1;
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = TRUE;
			}
		}
	}
}

void AddChestTraps()
{
	int i, j;
	char oi;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dObject[i][j] > 0) {
				oi = dObject[i][j] - 1;
				if (object[oi]._otype >= OBJ_CHEST1 && object[oi]._otype <= OBJ_CHEST3 && !object[oi]._oTrapFlag && random_(0, 100) < 10) {
					object[oi]._otype += OBJ_TCHEST1 - OBJ_CHEST1;
					object[oi]._oTrapFlag = TRUE;
					if (leveltype == DTYPE_CATACOMBS) {
						object[oi]._oVar4 = random_(0, 2);
					} else {
#ifdef HELLFIRE
						object[oi]._oVar4 = random_(0, 6);
#else
						object[oi]._oVar4 = random_(0, 3);
#endif
					}
				}
			}
		}
	}
}

void LoadMapObjects(const BYTE *pMap, int startx, int starty, int x1, int y1, int w, int h, int leveridx)
{
	int rw, rh, i, j, oi, type;
	const BYTE *lm;
	long mapoff;

	InitObjFlag = TRUE;

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += rw * 2 * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				type = *lm;
				AddObject(ObjTypeConv[type], startx + 16 + i, starty + 16 + j);
				oi = ObjIndex(startx + 16 + i, starty + 16 + j);
				SetObjMapRange(oi, x1, y1, x1 + w, y1 + h, leveridx);
			}
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}

void LoadMapObjs(const BYTE *pMap, int startx, int starty)
{
	int rw, rh;
	int i, j;
	const BYTE *lm;
	long mapoff;

	InitObjFlag = TRUE;
	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				AddObject(ObjTypeConv[*lm], startx + 16 + i, starty + 16 + j);
			}
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}

void AddDiabObjs()
{
	auto& levelsCache = asset::GlobalCache::Get().diabdat.levels;
	LoadMapObjects(levelsCache.l4data.diab1_dun.GetData(), 2 * diabquad1x, 2 * diabquad1y, diabquad2x, diabquad2y, 11, 12, 1);
	LoadMapObjects(levelsCache.l4data.diab2a_dun.GetData(), 2 * diabquad2x, 2 * diabquad2y, diabquad3x, diabquad3y, 11, 11, 2);
	LoadMapObjects(levelsCache.l4data.diab3a_dun.GetData(), 2 * diabquad3x, 2 * diabquad3y, diabquad4x, diabquad4y, 9, 9, 3);
}

#ifdef HELLFIRE
void AddCryptStoryBook(int s)
{
	DIABOOL exit;
	int xp, yp, cnt, m, n;

	cnt = 0;
	exit = FALSE;
	while (!exit) {
		exit = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (n = -2; n <= 2; n++) {
			for (m = -3; m <= 3; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = FALSE;
			}
		}
		if (!exit) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}
	AddCryptBook(OBJ_STORYBOOK, s, xp, yp);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp + 1);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp + 1);
}

void AddNakrulGate()
{
	AddNakrulLeaver();
	switch (random_(0, 6)) {
	case 0:
		AddNakrulBook(6, UberRow + 3, UberCol);
		AddNakrulBook(7, UberRow + 2, UberCol - 3);
		AddNakrulBook(8, UberRow + 2, UberCol + 2);
		break;
	case 1:
		AddNakrulBook(6, UberRow + 3, UberCol);
		AddNakrulBook(8, UberRow + 2, UberCol - 3);
		AddNakrulBook(7, UberRow + 2, UberCol + 2);
		break;
	case 2:
		AddNakrulBook(7, UberRow + 3, UberCol);
		AddNakrulBook(6, UberRow + 2, UberCol - 3);
		AddNakrulBook(8, UberRow + 2, UberCol + 2);
		break;
	case 3:
		AddNakrulBook(7, UberRow + 3, UberCol);
		AddNakrulBook(8, UberRow + 2, UberCol - 3);
		AddNakrulBook(6, UberRow + 2, UberCol + 2);
		break;
	case 4:
		AddNakrulBook(8, UberRow + 3, UberCol);
		AddNakrulBook(7, UberRow + 2, UberCol - 3);
		AddNakrulBook(6, UberRow + 2, UberCol + 2);
		break;
	case 5:
		AddNakrulBook(8, UberRow + 3, UberCol);
		AddNakrulBook(6, UberRow + 2, UberCol - 3);
		AddNakrulBook(7, UberRow + 2, UberCol + 2);
		break;
	}
}

void AddNakrulBook(int a1, int a2, int a3)
{
	AddCryptBook(OBJ_STORYBOOK, a1, a2, a3);
}
#endif

void AddStoryBooks()
{
	int xp, yp, xx, yy;
	int cnt;
	DIABOOL done;

	cnt = 0;
	done = FALSE;
	while (!done) {
		done = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -2; yy <= 2; yy++) {
			for (xx = -3; xx <= 3; xx++) {
				if (!RndLocOk(xx + xp, yy + yp))
					done = FALSE;
			}
		}
		if (!done) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}
	AddObject(OBJ_STORYBOOK, xp, yp);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp + 1);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp + 1);
}

void AddHookedBodies(int freq)
{
	int i, j, ii, jj;

	for (j = 0; j < DMAXY; j++) {
		jj = 16 + j * 2;
		for (i = 0; i < DMAXX; i++) {
			ii = 16 + i * 2;
			if (GetDungeon(i, j) != 1 && GetDungeon(i, j) != 2)
				continue;
			if (random_(0, freq) != 0)
				continue;
			if (!SkipThemeRoom(i, j))
				continue;
			if (GetDungeon(i, j) == 1 && GetDungeon(i + 1, j) == 6) {
				switch (random_(0, 3)) {
				case 0:
					AddObject(OBJ_TORTURE1, ii + 1, jj);
					break;
				case 1:
					AddObject(OBJ_TORTURE2, ii + 1, jj);
					break;
				case 2:
					AddObject(OBJ_TORTURE5, ii + 1, jj);
					break;
				}
				continue;
			}
			if (GetDungeon(i, j) == 2 && GetDungeon(i, j + 1) == 6) {
				switch (random_(0, 2)) {
				case 0:
					AddObject(OBJ_TORTURE3, ii, jj);
					break;
				case 1:
					AddObject(OBJ_TORTURE4, ii, jj);
					break;
				}
			}
		}
	}
}

void AddL4Goodies()
{
	AddHookedBodies(6);
	InitRndLocObj(2, 6, OBJ_TNUDEM1);
	InitRndLocObj(2, 6, OBJ_TNUDEM2);
	InitRndLocObj(2, 6, OBJ_TNUDEM3);
	InitRndLocObj(2, 6, OBJ_TNUDEM4);
	InitRndLocObj(2, 6, OBJ_TNUDEW1);
	InitRndLocObj(2, 6, OBJ_TNUDEW2);
	InitRndLocObj(2, 6, OBJ_TNUDEW3);
	InitRndLocObj(2, 6, OBJ_DECAP);
	InitRndLocObj(1, 3, OBJ_CAULDRON);
}

void AddLazStand()
{
	int xp, yp, xx, yy;
	int cnt;
	DIABOOL found;

	cnt = 0;
	found = FALSE;
	while (!found) {
		found = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -3; yy <= 3; yy++) {
			for (xx = -2; xx <= 3; xx++) {
				if (!RndLocOk(xp + xx, yp + yy))
					found = FALSE;
			}
		}
		if (!found) {
			cnt++;
			if (cnt > 10000) {
				InitRndLocObj(1, 1, OBJ_LAZSTAND);
				return;
			}
		}
	}
	AddObject(OBJ_LAZSTAND, xp, yp);
	AddObject(OBJ_TNUDEM2, xp, yp + 2);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp + 2);
	AddObject(OBJ_TNUDEM3, xp + 2, yp + 2);
	AddObject(OBJ_TNUDEW1, xp, yp - 2);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 2);
	AddObject(OBJ_TNUDEW2, xp + 2, yp - 2);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_TNUDEW3, xp - 1, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp + 1);
}

void InitObjects()
{
	int sp_id;
	const BYTE *mem;

	ClrAllObjects();
#ifdef HELLFIRE
	dword_6DE0E0 = 0;
#endif
	if (currlevel == 16) {
		AddDiabObjs();
	} else {
		InitObjFlag = TRUE;
		GetRndSeed();
		if (currlevel == 9 && gbMaxPlayers == 1)
			AddSlainHero();
		if (currlevel == quests[Q_MUSHROOM]._qlevel && quests[Q_MUSHROOM]._qactive == QUEST_INIT)
			AddMushPatch();

#ifdef HELLFIRE
		if (currlevel == 4 || currlevel == 8 || currlevel == 12)
			AddStoryBooks();
		if (currlevel == 21) {
			AddCryptStoryBook(1);
		} else if (currlevel == 22) {
			AddCryptStoryBook(2);
			AddCryptStoryBook(3);
		} else if (currlevel == 23) {
			AddCryptStoryBook(4);
			AddCryptStoryBook(5);
		}
		if (currlevel == 24) {
			AddNakrulGate();
		}
#else
		if (currlevel == 4)
			AddStoryBooks();
		if (currlevel == 8)
			AddStoryBooks();
		if (currlevel == 12)
			AddStoryBooks();
#endif
		auto& levelsCache = asset::GlobalCache::Get().diabdat.levels;
		if (leveltype == DTYPE_CATHEDRAL) {
			if (QuestStatus(Q_BUTCHER))
				AddTortures();
			if (QuestStatus(Q_PWATER))
				AddCandles();
			if (QuestStatus(Q_LTBANNER))
				AddObject(OBJ_SIGNCHEST, 2 * setpc_x + 26, 2 * setpc_y + 19);
			InitRndLocBigObj(10, 15, OBJ_SARC);
#ifdef HELLFIRE
			if (currlevel >= 21)
				add_crypt_objs(0, 0, MAXDUNX, MAXDUNY);
			else
#endif
				AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CATACOMBS) {
			if (QuestStatus(Q_ROCK))
				InitRndLocObj5x5(1, 1, OBJ_STAND);
			if (QuestStatus(Q_SCHAMB))
				InitRndLocObj5x5(1, 1, OBJ_BOOK2R);
			AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
			AddL2Torches();
			if (QuestStatus(Q_BLIND)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLINDING;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLINDING;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLINDING;
#ifdef HELLFIRE
				} else if (plr[myplr]._pClass == PC_MONK) {
					sp_id = TEXT_HBLINDING;
				} else if (plr[myplr]._pClass == PC_BARD) {
					sp_id = TEXT_BBLINDING;
				} else if (plr[myplr]._pClass == PC_BARBARIAN) {
					sp_id = TEXT_BLINDING;
#endif
				}
				quests[Q_BLIND]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1, sp_id);
				mem = levelsCache.l2data.blind2_dun.GetData();
				// BUGFIX: should not invoke LoadMapObjs for Blind2.DUN, as Blind2.DUN is missing an objects layer.
				LoadMapObjs(mem, 2 * setpc_x, 2 * setpc_y);
			}
			if (QuestStatus(Q_BLOOD)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLOODY;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLOODY;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLOODY;
#ifdef HELLFIRE
				} else if (plr[myplr]._pClass == PC_MONK) {
					sp_id = TEXT_HBLOODY;
				} else if (plr[myplr]._pClass == PC_BARD) {
					sp_id = TEXT_BBLOODY;
				} else if (plr[myplr]._pClass == PC_BARBARIAN) {
					sp_id = TEXT_BLOODY;
#endif
				}
				quests[Q_BLOOD]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7, sp_id);
				AddObject(OBJ_PEDISTAL, 2 * setpc_x + 25, 2 * setpc_y + 32);
			}
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CAVES) {
			AddL3Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_HELL) {
			if (QuestStatus(Q_WARLORD)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLOODWAR;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLOODWAR;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLOODWAR;
#ifdef HELLFIRE
				} else if (plr[myplr]._pClass == PC_MONK) {
					sp_id = TEXT_HBLOODWAR;
				} else if (plr[myplr]._pClass == PC_BARD) {
					sp_id = TEXT_BBLOODWAR;
				} else if (plr[myplr]._pClass == PC_BARBARIAN) {
					sp_id = TEXT_BLOODWAR;
#endif
				}
				quests[Q_WARLORD]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_x + setpc_w, setpc_y + setpc_h, sp_id);
				mem = levelsCache.l4data.warlord_dun.GetData();
				LoadMapObjs(mem, 2 * setpc_x, 2 * setpc_y);
			}
			if (QuestStatus(Q_BETRAYER) && gbMaxPlayers == 1)
				AddLazStand();
			InitRndBarrels();
			AddL4Goodies();
		}
		InitRndLocObj(5, 10, OBJ_CHEST1);
		InitRndLocObj(3, 6, OBJ_CHEST2);
		InitRndLocObj(1, 5, OBJ_CHEST3);
		if (leveltype != DTYPE_HELL)
			AddObjTraps();
		if (leveltype > DTYPE_CATHEDRAL)
			AddChestTraps();
		InitObjFlag = FALSE;
	}
}

#ifndef SPAWN
void SetMapObjects(const BYTE *pMap, int startx, int starty)
{
	int rw, rh;
	int i, j;
	const BYTE *lm;
	const BYTE *h;
	long mapoff;
	int fileload[56];
	char filestr[32];

	ClrAllObjects();
	for (i = 0; i < 56; i++)
		fileload[i] = FALSE;
	InitObjFlag = TRUE;

	for (i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1 && leveltype == AllObjects[i].olvltype)
			fileload[AllObjects[i].ofindex] = TRUE;
	}

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;
	h = lm;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				fileload[AllObjects[ObjTypeConv[*lm]].ofindex] = TRUE;
			}
			lm += 2;
		}
	}

	for (i = 0; i < 56; i++) {
		if (!fileload[i])
			continue;

		ObjFileList[numobjfiles] = i;
		sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
	#if 0 /* This is part of vanilla code, but this variable isn't used anywhere else. */
		pObjCels[numobjfiles] = LoadFileInMem(filestr, NULL);
	#endif
		numobjfiles++;
	}

	lm = h;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm)
				AddObject(ObjTypeConv[*lm], startx + 16 + i, starty + 16 + j);
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}
#endif

void DeleteObject_(int oi, int i)
{
	int ox, oy;

	ox = object[oi]._ox;
	oy = object[oi]._oy;
	dObject[ox][oy] = 0;
	objectavail[-nobjects + MAXOBJECTS] = oi;
	nobjects--;
	if (nobjects > 0 && i != nobjects)
		objectactive[i] = objectactive[nobjects];
}

void SetupObject(int i, int x, int y, int ot)
{
	int ofi;
	int j;

	object[i]._otype = ot;
	ofi = AllObjects[ot].ofindex;
	object[i]._ox = x;
	object[i]._oy = y;
	object[i]._oAnimFlag = AllObjects[ot].oAnimFlag;
	if (AllObjects[ot].oAnimFlag) {
		object[i]._oAnimDelay = AllObjects[ot].oAnimDelay;
		object[i]._oAnimCnt = random_(146, AllObjects[ot].oAnimDelay);
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = random_(146, AllObjects[ot].oAnimLen - 1) + 1;
	} else {
		object[i]._oAnimDelay = 1000;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = AllObjects[ot].oAnimDelay;
	}
	object[i]._oAnimWidth = AllObjects[ot].oAnimWidth;
	object[i]._oSolidFlag = AllObjects[ot].oSolidFlag;
	object[i]._oMissFlag = AllObjects[ot].oMissFlag;
	object[i]._oLight = AllObjects[ot].oLightFlag;
	object[i]._oDelFlag = FALSE;
	object[i]._oBreak = AllObjects[ot].oBreak;
	object[i]._oSelFlag = AllObjects[ot].oSelFlag;
	object[i]._oPreFlag = FALSE;
	object[i]._oTrapFlag = FALSE;
	object[i]._oDoorFlag = FALSE;
}

void SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v)
{
	object[i]._oVar1 = x1;
	object[i]._oVar2 = y1;
	object[i]._oVar3 = x2;
	object[i]._oVar4 = y2;
	object[i]._oVar8 = v;
}

void SetBookMsg(int i, int msg)
{
	object[i]._oVar7 = msg;
}

void AddL1Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
	if (ot == 1) {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x][y - 1];
	} else {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x - 1][y];
	}
	object[i]._oVar4 = 0;
}

void AddSCambBook(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_w + setpc_x + 1;
	object[i]._oVar4 = setpc_h + setpc_y + 1;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}

void AddChest(int i, int t)
{
	if (random_(147, 2) == 0)
		object[i]._oAnimFrame += 3;
	object[i]._oRndSeed = GetRndSeed();
	switch (t) {
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		if (setlevel) {
			object[i]._oVar1 = 1;
			break;
		}
		object[i]._oVar1 = random_(147, 2);
		break;
	case OBJ_TCHEST2:
	case OBJ_CHEST2:
		if (setlevel) {
			object[i]._oVar1 = 2;
			break;
		}
		object[i]._oVar1 = random_(147, 3);
		break;
	case OBJ_TCHEST3:
	case OBJ_CHEST3:
		if (setlevel) {
			object[i]._oVar1 = 3;
			break;
		}
		object[i]._oVar1 = random_(147, 4);
		break;
	}
	object[i]._oVar2 = random_(147, 8);
}

void AddL2Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L2LDOOR)
		ObjSetMicro(x, y, 538);
	else
		ObjSetMicro(x, y, 540);
	object[i]._oVar4 = 0;
}

void AddL3Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L3LDOOR)
		ObjSetMicro(x, y, 531);
	else
		ObjSetMicro(x, y, 534);
	object[i]._oVar4 = 0;
}

void AddSarc(int i)
{
	dObject[object[i]._ox][object[i]._oy - 1] = -(i + 1);
	object[i]._oVar1 = random_(153, 10);
	object[i]._oRndSeed = GetRndSeed();
	if (object[i]._oVar1 >= 8)
		object[i]._oVar2 = PreSpawnSkeleton();
}

void AddFlameTrap(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = 0;
	object[i]._oVar3 = trapdir;
	object[i]._oVar4 = 0;
}

void AddFlameLvr(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = MIS_FLAMEC;
}

void AddTrap(int i, int ot)
{
	int mt;

	mt = currlevel / 3 + 1;
#ifdef HELLFIRE
	if (currlevel > 16) {
		mt = (currlevel - 4) / 3 + 1;
	}
	if (currlevel > 20) {
		mt = (currlevel - 8) / 3 + 1;
	}
#endif
	mt = random_(148, mt);
	if (mt == 0)
		object[i]._oVar3 = MIS_ARROW;
	if (mt == 1)
		object[i]._oVar3 = MIS_FIREBOLT;
	if (mt == 2)
		object[i]._oVar3 = MIS_LIGHTCTRL;
	object[i]._oVar4 = 0;
}

void AddObjLight(int i, int r)
{
	if (InitObjFlag) {
		DoLighting(object[i]._ox, object[i]._oy, r, -1);
		object[i]._oVar1 = -1;
	} else {
		object[i]._oVar1 = 0;
	}
}

void AddBarrel(int i, int t)
{
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oVar2 = random_(149, 10);
	object[i]._oVar3 = random_(149, 3);

	if (object[i]._oVar2 >= 8)
		object[i]._oVar4 = PreSpawnSkeleton();
}

void AddShrine(int i)
{
	int val;
	DIABOOL slist[NUM_SHRINETYPE];
#ifdef HELLFIRE
	unsigned int j;
#else
	int j;
#endif

	// BUGFIX: the seed of shrine objects (object[i]._oRndSeed) was never
	// initialized. This lead to undefined behaviour, as the shrine object would
	// use whatever value was present in memory (often the seed of an object with
	// the same object index of a previous dungeon level).

	object[i]._oPreFlag = TRUE;
	for (j = 0; j < NUM_SHRINETYPE; j++) {
		if (currlevel < shrinemin[j] || currlevel > shrinemax[j]) {
			slist[j] = 0;
		} else {
			slist[j] = 1;
		}
		if (gbMaxPlayers != 1 && shrineavail[j] == 1) {
			slist[j] = 0;
		}
		if (gbMaxPlayers == 1 && shrineavail[j] == 2) {
			slist[j] = 0;
		}
	}
	do {
		val = random_(150, NUM_SHRINETYPE);
	} while (!slist[val]);

	object[i]._oVar1 = val;
	if (random_(150, 2) != 0) {
		object[i]._oAnimFrame = 12;
		object[i]._oAnimLen = 22;
	}
}

void AddBookcase(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
}

void AddBookstand(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddBloodFtn(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPurifyingFountain(int i)
{
	int ox, oy;

	ox = object[i]._ox;
	oy = object[i]._oy;
	dObject[ox][oy - 1] = -1 - i;
	dObject[ox - 1][oy] = -1 - i;
	dObject[ox - 1][oy - 1] = -1 - i;
	object[i]._oRndSeed = GetRndSeed();
}

void AddArmorStand(int i)
{
	if (!armorFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}

	object[i]._oRndSeed = GetRndSeed();
}

void AddGoatShrine(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddCauldron(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddMurkyFountain(int i)
{
	int ox, oy;

	ox = object[i]._ox;
	oy = object[i]._oy;
	dObject[ox][oy - 1] = -1 - i;
	dObject[ox - 1][oy] = -1 - i;
	dObject[ox - 1][oy - 1] = -1 - i;
	object[i]._oRndSeed = GetRndSeed();
}

void AddTearFountain(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddDecap(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(151, 8) + 1;
	object[i]._oPreFlag = TRUE;
}

void AddVilebook(int i)
{
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		object[i]._oAnimFrame = 4;
	}
}

void AddMagicCircle(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
	object[i]._oVar6 = 0;
	object[i]._oVar5 = 1;
}

void AddBrnCross(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPedistal(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_x + setpc_w;
	object[i]._oVar4 = setpc_y + setpc_h;
}

void AddStoryBook(int i)
{
	SetRndSeed(glSeedTbl[16]);

	object[i]._oVar1 = random_(0, 3);
	if (currlevel == 4)
		object[i]._oVar2 = StoryText[object[i]._oVar1][0];
#ifdef HELLFIRE
	if (currlevel == 8)
#else
	else if (currlevel == 8)
#endif
		object[i]._oVar2 = StoryText[object[i]._oVar1][1];
#ifdef HELLFIRE
	if (currlevel == 12)
#else
	else if (currlevel == 12)
#endif
		object[i]._oVar2 = StoryText[object[i]._oVar1][2];
	object[i]._oVar3 = (currlevel >> 2) + 3 * object[i]._oVar1 - 1;
	object[i]._oAnimFrame = 5 - 2 * object[i]._oVar1;
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

void AddWeaponRack(int i)
{
	if (!weaponFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}
	object[i]._oRndSeed = GetRndSeed();
}

void AddTorturedBody(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(0, 4) + 1;
	object[i]._oPreFlag = TRUE;
}

void GetRndObjLoc(int randarea, int &xx, int &yy)
{
	DIABOOL failed;
	int i, j, tries;

	if (randarea == 0)
		return;

	tries = 0;
	while (1) {
		tries++;
		if (tries > 1000 && randarea > 1)
			randarea--;
		xx = random_(0, MAXDUNX);
		yy = random_(0, MAXDUNY);
		failed = FALSE;
		for (i = 0; i < randarea && !failed; i++) {
			for (j = 0; j < randarea && !failed; j++) {
				failed = !RndLocOk(i + xx, j + yy);
			}
		}
		if (!failed)
			break;
	}
}

void AddMushPatch()
{
	int i;
	int y, x;

	if (nobjects < MAXOBJECTS) {
		i = objectavail[0];
		GetRndObjLoc(5, x, y);
		dObject[x + 1][y + 1] = -1 - i;
		dObject[x + 2][y + 1] = -1 - i;
		dObject[x + 1][y + 2] = -1 - i;
		AddObject(OBJ_MUSHPATCH, x + 2, y + 2);
	}
}

void AddSlainHero()
{
	int x, y;

	GetRndObjLoc(5, x, y);
	AddObject(OBJ_SLAINHERO, x + 2, y + 2);
}

#ifdef HELLFIRE
void AddCryptBook(int ot, int v2, int ox, int oy)
{
	int oi;

	if (nobjects >= MAXOBJECTS)
		return;

	oi = objectavail[0];
	objectavail[0] = objectavail[MAXOBJECTS - 1 - nobjects];
	objectactive[nobjects] = oi;
	dObject[ox][oy] = oi + 1;
	SetupObject(oi, ox, oy, ot);
	AddCryptObject(oi, v2);
	object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
	nobjects++;
}

void AddCryptObject(int i, int a2)
{
	int v8, v9;
	if (a2 > 5) {
		object[i]._oVar8 = a2;
		switch (a2) {
		case 6:
			if (plr[myplr]._pClass == PC_WARRIOR) {
				object[i]._oVar2 = 323;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				object[i]._oVar2 = 332;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				object[i]._oVar2 = 329;
			} else if (plr[myplr]._pClass == PC_MONK) {
				object[i]._oVar2 = 326;
			} else if (plr[myplr]._pClass == PC_BARD) {
				object[i]._oVar2 = 335;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				object[i]._oVar2 = 323;
			}
			break;
		case 7:
			if (plr[myplr]._pClass == PC_WARRIOR) {
				object[i]._oVar2 = 324;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				object[i]._oVar2 = 333;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				object[i]._oVar2 = 330;
			} else if (plr[myplr]._pClass == PC_MONK) {
				object[i]._oVar2 = 327;
			} else if (plr[myplr]._pClass == PC_BARD) {
				object[i]._oVar2 = 336;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				object[i]._oVar2 = 324;
			}
			break;
		case 8:
			if (plr[myplr]._pClass == PC_WARRIOR) {
				object[i]._oVar2 = 325;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				object[i]._oVar2 = 334;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				object[i]._oVar2 = 331;
			} else if (plr[myplr]._pClass == PC_MONK) {
				object[i]._oVar2 = 328;
			} else if (plr[myplr]._pClass == PC_BARD) {
				object[i]._oVar2 = 337;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				object[i]._oVar2 = 325;
			}
			break;
		}
		object[i]._oVar1 = 1;
		object[i]._oVar3 = 15;
		v8 = 2 * object[i]._oVar1;
		object[i]._oAnimFrame = 5 - v8;
		object[i]._oVar4 = object[i]._oAnimFrame + 1;
	} else {

		object[i]._oVar1 = 1;
		object[i]._oVar2 = a2 + 316;
		object[i]._oVar3 = a2 + 9;
		v9 = 2 * object[i]._oVar1;
		object[i]._oAnimFrame = 5 - v9;
		object[i]._oVar4 = object[i]._oAnimFrame + 1;
		object[i]._oVar8 = 0;
	}
}
#endif

void AddObject(int ot, int ox, int oy)
{
	int oi;

	if (nobjects >= MAXOBJECTS)
		return;

	oi = objectavail[0];
	objectavail[0] = objectavail[MAXOBJECTS - 1 - nobjects];
	objectactive[nobjects] = oi;
	dObject[ox][oy] = oi + 1;
	SetupObject(oi, ox, oy, ot);
	switch (ot) {
	case OBJ_L1LIGHT:
		AddObjLight(oi, 5);
		break;
	case OBJ_SKFIRE:
	case OBJ_CANDLE1:
	case OBJ_CANDLE2:
	case OBJ_BOOKCANDLE:
		AddObjLight(oi, 5);
		break;
	case OBJ_STORYCANDLE:
		AddObjLight(oi, 3);
		break;
	case OBJ_TORCHL:
	case OBJ_TORCHR:
	case OBJ_TORCHL2:
	case OBJ_TORCHR2:
		AddObjLight(oi, 8);
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		AddL1Door(oi, ox, oy, ot);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		AddL2Door(oi, ox, oy, ot);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		AddL3Door(oi, ox, oy, ot);
		break;
	case OBJ_BOOK2R:
		AddSCambBook(oi);
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		AddChest(oi, ot);
		break;
	case OBJ_SARC:
		AddSarc(oi);
		break;
	case OBJ_FLAMEHOLE:
		AddFlameTrap(oi);
		break;
	case OBJ_FLAMELVR:
		AddFlameLvr(oi);
		break;
	case OBJ_WATER:
		object[oi]._oAnimFrame = 1;
		break;
	case OBJ_TRAPL:
	case OBJ_TRAPR:
		AddTrap(oi, ot);
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		AddBarrel(oi, ot);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		AddShrine(oi);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		AddBookcase(oi);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		AddBookstand(oi);
		break;
	case OBJ_BLOODFTN:
		AddBloodFtn(oi);
		break;
	case OBJ_DECAP:
		AddDecap(oi);
		break;
	case OBJ_PURIFYINGFTN:
		AddPurifyingFountain(oi);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		AddArmorStand(oi);
		break;
	case OBJ_GOATSHRINE:
		AddGoatShrine(oi);
		break;
	case OBJ_CAULDRON:
		AddCauldron(oi);
		break;
	case OBJ_MURKYFTN:
		AddMurkyFountain(oi);
		break;
	case OBJ_TEARFTN:
		AddTearFountain(oi);
		break;
	case OBJ_BOOK2L:
		AddVilebook(oi);
		break;
	case OBJ_MCIRCLE1:
	case OBJ_MCIRCLE2:
		AddMagicCircle(oi);
		break;
	case OBJ_STORYBOOK:
		AddStoryBook(oi);
		break;
	case OBJ_BCROSS:
	case OBJ_TBCROSS:
		AddBrnCross(oi);
		AddObjLight(oi, 5);
		break;
	case OBJ_PEDISTAL:
		AddPedistal(oi);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		AddWeaponRack(oi);
		break;
	case OBJ_TNUDEM2:
		AddTorturedBody(oi);
		break;
	}
	object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
	nobjects++;
}

void ObjSetMicro(int dx, int dy, int pn)
{
	WORD *v;
	MICROS *defs;
	int i;

	dPiece[dx][dy] = pn;
	pn--;
	defs = &dpiece_defs_map_1[IsometricCoord(dx, dy)];
	if (leveltype != DTYPE_HELL) {
		v = (WORD *)pLevelPieces + 10 * pn;
		for (i = 0; i < 10; i++) {
			defs->mt[i] = v[(i & 1) - (i & 0xE) + 8];
		}
	} else {
		v = (WORD *)pLevelPieces + 16 * pn;
		for (i = 0; i < 16; i++) {
			defs->mt[i] = v[(i & 1) - (i & 0xE) + 14];
		}
	}
}

void objects_set_door_piece(int x, int y)
{
	int pn;
	long v1, v2;

	pn = dPiece[x][y] - 1;

#ifdef USE_ASM
	__asm {
	mov		esi, pLevelPieces
	xor		eax, eax
	mov		ax, word ptr pn
	mov		ebx, 20
	mul		ebx
	add		esi, eax
	add		esi, 16
	xor		eax, eax
	lodsw
	mov		word ptr v1, ax
	lodsw
	mov		word ptr v2, ax
	}
#else
	v1 = *((WORD *)pLevelPieces + 10 * pn + 8);
	v2 = *((WORD *)pLevelPieces + 10 * pn + 9);
#endif
	dpiece_defs_map_1[IsometricCoord(x, y)].mt[0] = v1;
	dpiece_defs_map_1[IsometricCoord(x, y)].mt[1] = v2;
}

void ObjSetMini(int x, int y, int v)
{
	int xx, yy;
	long v1, v2, v3, v4;

#ifdef USE_ASM
	__asm {
		mov		esi, pMegaTiles
		xor		eax, eax
		mov		ax, word ptr v
		dec		eax
		shl		eax, 3
		add		esi, eax
		xor		eax, eax
		lodsw
		inc		eax
		mov		v1, eax
		lodsw
		inc		eax
		mov		v2, eax
		lodsw
		inc		eax
		mov		v3, eax
		lodsw
		inc		eax
		mov		v4, eax
	}
#else
	v1 = *((WORD *)&pMegaTiles[((WORD)v - 1) * 8]) + 1;
	v2 = *((WORD *)&pMegaTiles[((WORD)v - 1) * 8] + 1) + 1;
	v3 = *((WORD *)&pMegaTiles[((WORD)v - 1) * 8] + 2) + 1;
	v4 = *((WORD *)&pMegaTiles[((WORD)v - 1) * 8] + 3) + 1;
#endif

	xx = 2 * x + 16;
	yy = 2 * y + 16;
	ObjSetMicro(xx, yy, v1);
	ObjSetMicro(xx + 1, yy, v2);
	ObjSetMicro(xx, yy + 1, v3);
	ObjSetMicro(xx + 1, yy + 1, v4);
}

void ObjL1Special(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (i = y1; i <= y2; ++i) {
		for (j = x1; j <= x2; ++j) {
			dSpecial[j][i] = 0;
			if (dPiece[j][i] == 12)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 11)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 71)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 259)
				dSpecial[j][i] = 5;
			if (dPiece[j][i] == 249)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 325)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 321)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 255)
				dSpecial[j][i] = 4;
			if (dPiece[j][i] == 211)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 344)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 341)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 331)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 418)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 421)
				dSpecial[j][i] = 2;
		}
	}
}

void ObjL2Special(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dSpecial[i][j] = 0;
			if (dPiece[i][j] == 541)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 178)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 551)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 542)
				dSpecial[i][j] = 6;
			if (dPiece[i][j] == 553)
				dSpecial[i][j] = 6;
			if (dPiece[i][j] == 13)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 17)
				dSpecial[i][j] = 6;
		}
	}
	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j + 1] = 2;
				dSpecial[i][j + 2] = 1;
			}
			if (dPiece[i][j] == 135 || dPiece[i][j] == 139) {
				dSpecial[i + 1][j] = 3;
				dSpecial[i + 2][j] = 4;
			}
		}
	}
}

void DoorSet(int oi, int dx, int dy)
{
	int pn;

	pn = dPiece[dx][dy];
#ifdef HELLFIRE
	if (currlevel < 17) {
#endif
		if (pn == 43)
			ObjSetMicro(dx, dy, 392);
		if (pn == 45)
			ObjSetMicro(dx, dy, 394);
		if (pn == 50 && object[oi]._otype == OBJ_L1LDOOR)
			ObjSetMicro(dx, dy, 411);
		if (pn == 50 && object[oi]._otype == OBJ_L1RDOOR)
			ObjSetMicro(dx, dy, 412);
		if (pn == 54)
			ObjSetMicro(dx, dy, 397);
		if (pn == 55)
			ObjSetMicro(dx, dy, 398);
		if (pn == 61)
			ObjSetMicro(dx, dy, 399);
		if (pn == 67)
			ObjSetMicro(dx, dy, 400);
		if (pn == 68)
			ObjSetMicro(dx, dy, 401);
		if (pn == 69)
			ObjSetMicro(dx, dy, 403);
		if (pn == 70)
			ObjSetMicro(dx, dy, 404);
		if (pn == 72)
			ObjSetMicro(dx, dy, 406);
		if (pn == 212)
			ObjSetMicro(dx, dy, 407);
		if (pn == 354)
			ObjSetMicro(dx, dy, 409);
		if (pn == 355)
			ObjSetMicro(dx, dy, 410);
		if (pn == 411)
			ObjSetMicro(dx, dy, 396);
		if (pn == 412)
			ObjSetMicro(dx, dy, 396);
#ifdef HELLFIRE
	} else {
		if (pn == 75)
			ObjSetMicro(dx, dy, 204);
		if (pn == 79)
			ObjSetMicro(dx, dy, 208);
		if (pn == 86 && object[oi]._otype == OBJ_L1LDOOR) {
			ObjSetMicro(dx, dy, 232);
		}
		if (pn == 86 && object[oi]._otype == OBJ_L1RDOOR) {
			ObjSetMicro(dx, dy, 234);
		}
		if (pn == 91)
			ObjSetMicro(dx, dy, 215);
		if (pn == 93)
			ObjSetMicro(dx, dy, 218);
		if (pn == 99)
			ObjSetMicro(dx, dy, 220);
		if (pn == 111)
			ObjSetMicro(dx, dy, 222);
		if (pn == 113)
			ObjSetMicro(dx, dy, 224);
		if (pn == 115)
			ObjSetMicro(dx, dy, 226);
		if (pn == 117)
			ObjSetMicro(dx, dy, 228);
		if (pn == 119)
			ObjSetMicro(dx, dy, 230);
		if (pn == 232)
			ObjSetMicro(dx, dy, 212);
		if (pn == 234)
			ObjSetMicro(dx, dy, 212);
	}
#endif
}

void RedoPlayerVision()
{
	int p;

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].plractive && currlevel == plr[p].plrlevel) {
			ChangeVisionXY(plr[p]._pvid, plr[p]._px, plr[p]._py);
		}
	}
}

void ObjChangeMap(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			ObjSetMini(i, j, pdungeon[i][j]);
			SetDungeon(i, j, pdungeon[i][j]);
		}
	}
#ifdef HELLFIRE
	if (leveltype == DTYPE_CATHEDRAL && currlevel < 17) {
#else
	if (leveltype == DTYPE_CATHEDRAL) {
#endif
		ObjL1Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
		AddL1Objs(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
	if (leveltype == DTYPE_CATACOMBS) {
		ObjL2Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
		AddL2Objs(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
}

void ObjChangeMapResync(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			ObjSetMini(i, j, pdungeon[i][j]);
			SetDungeon(i, j, pdungeon[i][j]);
		}
	}
#ifdef HELLFIRE
	if (leveltype == DTYPE_CATHEDRAL && currlevel < 17) {
#else
	if (leveltype == DTYPE_CATHEDRAL) {
#endif
		ObjL1Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
	if (leveltype == DTYPE_CATACOMBS) {
		ObjL2Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
}

int FindValidShrine(int i)
{
	int rv;
	DIABOOL done;

	done = FALSE;
	do {
		rv = random_(0, NUM_SHRINETYPE);
		if (currlevel >= shrinemin[rv] && currlevel <= shrinemax[rv] && rv != SHRINE_THAUMATURGIC) {
			done = TRUE;
		}
		if (done) {
			if (gbMaxPlayers != 1) {
				if (shrineavail[rv] == 1) {
					done = FALSE;
					continue;
				}
			}
			if (gbMaxPlayers == 1) {
				if (shrineavail[rv] == 2) {
					done = FALSE;
					continue;
				}
			}
			done = TRUE;
		}
	} while (!done);
	return rv;
}

void GetObjectStr(int i, char infostr[50])
{
	char tempstr[50];
	switch (object[i]._otype) {
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		strcpy(infostr, "Crucified Skeleton");
		break;
	case OBJ_LEVER:
	case OBJ_FLAMELVR:
		strcpy(infostr, "Lever");
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (object[i]._oVar4 == 1)
			strcpy(infostr, "Open Door");
		if (object[i]._oVar4 == 0)
			strcpy(infostr, "Closed Door");
		if (object[i]._oVar4 == 2)
			strcpy(infostr, "Blocked Door");
		break;
	case OBJ_BOOK2L:
		if (setlevel) {
			if (setlvlnum == SL_BONECHAMB) {
				strcpy(infostr, "Ancient Tome");
			} else if (setlvlnum == SL_VILEBETRAYER) {
				strcpy(infostr, "Book of Vileness");
			}
		}
		break;
	case OBJ_SWITCHSKL:
		strcpy(infostr, "Skull Lever");
		break;
	case OBJ_BOOK2R:
		strcpy(infostr, "Mythical Book");
		break;
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		strcpy(infostr, "Small Chest");
		break;
	case OBJ_CHEST2:
	case OBJ_TCHEST2:
		strcpy(infostr, "Chest");
		break;
	case OBJ_CHEST3:
	case OBJ_TCHEST3:
	case OBJ_SIGNCHEST:
		strcpy(infostr, "Large Chest");
		break;
	case OBJ_SARC:
		strcpy(infostr, "Sarcophagus");
		break;
	case OBJ_BOOKSHELF:
		strcpy(infostr, "Bookshelf");
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		strcpy(infostr, "Bookcase");
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
#ifdef HELLFIRE
		if (currlevel >= 17 && currlevel <= 20)      // for hive levels
			strcpy(infostr, "Pod");                  //Then a barrel is called a pod
		else if (currlevel >= 21 && currlevel <= 24) // for crypt levels
			strcpy(infostr, "Urn");                  //Then a barrel is called an urn
		else
#endif
			strcpy(infostr, "Barrel");
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		sprintf(tempstr, "%s Shrine", shrinestrs[object[i]._oVar1]);
		strcpy(infostr, tempstr);
		break;
	case OBJ_SKELBOOK:
		strcpy(infostr, "Skeleton Tome");
		break;
	case OBJ_BOOKSTAND:
		strcpy(infostr, "Library Book");
		break;
	case OBJ_BLOODFTN:
		strcpy(infostr, "Blood Fountain");
		break;
	case OBJ_DECAP:
		strcpy(infostr, "Decapitated Body");
		break;
	case OBJ_BLINDBOOK:
		strcpy(infostr, "Book of the Blind");
		break;
	case OBJ_BLOODBOOK:
		strcpy(infostr, "Book of Blood");
		break;
	case OBJ_PURIFYINGFTN:
		strcpy(infostr, "Purifying Spring");
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		strcpy(infostr, "Armor");
		break;
	case OBJ_WARWEAP:
		strcpy(infostr, "Weapon Rack");
		break;
	case OBJ_GOATSHRINE:
		strcpy(infostr, "Goat Shrine");
		break;
	case OBJ_CAULDRON:
		strcpy(infostr, "Cauldron");
		break;
	case OBJ_MURKYFTN:
		strcpy(infostr, "Murky Pool");
		break;
	case OBJ_TEARFTN:
		strcpy(infostr, "Fountain of Tears");
		break;
	case OBJ_STEELTOME:
		strcpy(infostr, "Steel Tome");
		break;
	case OBJ_PEDISTAL:
		strcpy(infostr, "Pedestal of Blood");
		break;
	case OBJ_STORYBOOK:
		strcpy(infostr, StoryBookName[object[i]._oVar3]);
		break;
	case OBJ_WEAPONRACK:
		strcpy(infostr, "Weapon Rack");
		break;
	case OBJ_MUSHPATCH:
		strcpy(infostr, "Mushroom Patch");
		break;
	case OBJ_LAZSTAND:
		strcpy(infostr, "Vile Stand");
		break;
	case OBJ_SLAINHERO:
		strcpy(infostr, "Slain Hero");
		break;
	}
	if (plr[myplr]._pClass == PC_ROGUE) {
		if (object[i]._oTrapFlag) {
			sprintf(tempstr, "Trapped %s", infostr);
			strcpy(infostr, tempstr);
		}
	}
}
