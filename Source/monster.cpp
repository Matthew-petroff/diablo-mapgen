/**
 * @file monster.cpp
 *
 * Implementation of monster functionality, AI, actions, spawning, loading, etc.
 */
#include "all.h"

/** Tracks which missile files are already loaded */
int MissileFileFlag;

// BUGFIX: replace monstkills[MAXMONSTERS] with monstkills[NUM_MTYPES].
/** Tracks the total number of monsters killed per monster_id. */
int monstkills[MAXMONSTERS];
int monstactive[MAXMONSTERS];
int nummonsters;
BOOLEAN sgbSaveSoundOn;
MonsterStruct monster[MAXMONSTERS];
int totalmonsters;
CMonster Monsters[MAX_LVLMTYPES];
#ifdef HELLFIRE
int GraphicTable[NUMLEVELS][MAX_LVLMTYPES];
#else
BYTE GraphicTable[NUMLEVELS][MAX_LVLMTYPES];
#endif
int monstimgtot;
int uniquetrans;
int nummtypes;

/** Maps from walking path step to facing direction. */
const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
/** Maps from monster intelligence factor to missile type. */
const BYTE counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };

/* data */

// BUGFIX: MWVel velocity values are not rounded consistently. The correct
// formula for monster walk velocity is calculated as follows (for 16, 32 and 64
// pixel distances, respectively):
//
//    vel16 = (16 << monsterWalkShift) / nframes
//    vel32 = (32 << monsterWalkShift) / nframes
//    vel64 = (64 << monsterWalkShift) / nframes
//
// The correct monster walk velocity table is as follows:
//
//   int MWVel[24][3] = {
//      { 256, 512, 1024 },
//      { 128, 256, 512 },
//      { 85, 171, 341 },
//      { 64, 128, 256 },
//      { 51, 102, 205 },
//      { 43, 85, 171 },
//      { 37, 73, 146 },
//      { 32, 64, 128 },
//      { 28, 57, 114 },
//      { 26, 51, 102 },
//      { 23, 47, 93 },
//      { 21, 43, 85 },
//      { 20, 39, 79 },
//      { 18, 37, 73 },
//      { 17, 34, 68 },
//      { 16, 32, 64 },
//      { 15, 30, 60 },
//      { 14, 28, 57 },
//      { 13, 27, 54 },
//      { 13, 26, 51 },
//      { 12, 24, 49 },
//      { 12, 23, 47 },
//      { 11, 22, 45 },
//      { 11, 21, 43 }
//   };

/** Maps from monster walk animation frame num to monster velocity. */
int MWVel[24][3] = {
	{ 256, 512, 1024 },
	{ 128, 256, 512 },
	{ 85, 170, 341 },
	{ 64, 128, 256 },
	{ 51, 102, 204 },
	{ 42, 85, 170 },
	{ 36, 73, 146 },
	{ 32, 64, 128 },
	{ 28, 56, 113 },
	{ 26, 51, 102 },
	{ 23, 46, 93 },
	{ 21, 42, 85 },
	{ 19, 39, 78 },
	{ 18, 36, 73 },
	{ 17, 34, 68 },
	{ 16, 32, 64 },
	{ 15, 30, 60 },
	{ 14, 28, 57 },
	{ 13, 26, 54 },
	{ 12, 25, 51 },
	{ 12, 24, 48 },
	{ 11, 23, 46 },
	{ 11, 22, 44 },
	{ 10, 21, 42 }
};
/** Maps from monster action to monster animation letter. */
char animletter[7] = "nwahds";
/** Maps from direction to a left turn from the direction. */
int left[8] = { 7, 0, 1, 2, 3, 4, 5, 6 };
/** Maps from direction to a right turn from the direction. */
int right[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };
/** Maps from direction to the opposite direction. */
int opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
/** Maps from direction to delta X-offset. */
int offset_x[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
/** Maps from direction to delta Y-offset. */
int offset_y[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

#ifdef HELLFIRE
int HorkXAdd[8] = { 1, 0, -1, -1, -1, 0, 1, 1 }; // CODEFIX: same values as offset_x, remove it and use offset_x instead
int HorkYAdd[8] = { 1, 1, 1, 0, -1, -1, -1, 0 }; // CODEFIX: same values as offset_y, remove it and use offset_y instead
#endif

/** unused */
int rnd5[4] = { 5, 10, 15, 20 };
int rnd10[4] = { 10, 15, 20, 30 };
int rnd20[4] = { 20, 30, 40, 50 };
int rnd60[4] = { 60, 70, 80, 90 };

///** Maps from monster AI ID to monster AI function. */
//void (*AiProc[])(int i) = {
//	&MAI_Zombie,
//	&MAI_Fat,
//	&MAI_SkelSd,
//	&MAI_SkelBow,
//	&MAI_Scav,
//	&MAI_Rhino,
//	&MAI_GoatMc,
//	&MAI_GoatBow,
//	&MAI_Fallen,
//	&MAI_Magma,
//	&MAI_SkelKing,
//	&MAI_Bat,
//	&MAI_Garg,
//	&MAI_Cleaver,
//	&MAI_Succ,
//	&MAI_Sneak,
//	&MAI_Storm,
//	&MAI_Fireman,
//	&MAI_Garbud,
//	&MAI_Acid,
//	&MAI_AcidUniq,
//	&MAI_Golum,
//	&MAI_Zhar,
//	&MAI_SnotSpil,
//	&MAI_Snake,
//	&MAI_Counselor,
//	&MAI_Mega,
//	&MAI_Diablo,
//	&MAI_Lazurus,
//	&MAI_Lazhelp,
//	&MAI_Lachdanan,
//	&MAI_Warlord,
//#ifdef HELLFIRE
//	&MAI_Firebat,
//	&MAI_Torchant,
//	&MAI_HorkDemon,
//	&MAI_Lich,
//	&MAI_ArchLich,
//	&MAI_Psychorb,
//	&MAI_Necromorb,
//	&MAI_BoneDemon
//#endif
//};

void InitLevelMonsters()
{
	ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;

	for (int i = 0; i < MAXMONSTERS; i++) {
		monstactive[i] = i;
	}

	uniquetrans = 0;
}

int AddMonsterType(int type, int placeflag)
{
	BOOL done = FALSE;
	int i;

	for (i = 0; i < nummtypes && !done; i++) {
		done = Monsters[i].mtype == type;
	}

	i--;

	if (!done) {
		i = nummtypes;
		nummtypes++;
		Monsters[i].mtype = type;
		monstimgtot += monsterdata[type].mImage;
		InitMonsterGFX(i);
	}

	Monsters[i].mPlaceFlags |= placeflag;
	return i;
}

void GetLevelMTypes()
{
	int i;

	for (i = 0; i < MAX_LVLMTYPES; i++) {
		Monsters[i].mPlaceFlags = 0;
	}
	
	nummtypes = 0;
	monstimgtot = 0;
	MissileFileFlag = 0;

	// this array is merged with skeltypes down below.
	int typelist[MAXMONSTERS];
	int skeltypes[NUM_MTYPES];

	int minl; // min level
	int maxl; // max level
	char mamask;
	const int numskeltypes = 19;

	int nt; // number of types

#ifdef SPAWN
	mamask = 1; // monster availability mask
#else
	mamask = 3; // monster availability mask
#endif

	AddMonsterType(MT_GOLEM, PLACE_SPECIAL);
	if (currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, PLACE_SCATTER);
		AddMonsterType(MT_RBLACK, PLACE_SCATTER);
		AddMonsterType(MT_DIABLO, PLACE_SPECIAL);
		return;
	}

#ifdef HELLFIRE
	if (currlevel == 18)
		AddMonsterType(MT_HORKSPWN, PLACE_SCATTER);
	if (currlevel == 19) {
		AddMonsterType(MT_HORKSPWN, PLACE_SCATTER);
		AddMonsterType(MT_HORKDMN, PLACE_UNIQUE);
	}
	if (currlevel == 20)
		AddMonsterType(MT_DEFILER, PLACE_UNIQUE);
	if (currlevel == 24) {
		AddMonsterType(MT_ARCHLICH, PLACE_SCATTER);
		AddMonsterType(MT_NAKRUL, PLACE_SPECIAL);
	}
#endif

	if (!setlevel) {
		if (QuestStatus(Q_BUTCHER))
			AddMonsterType(MT_CLEAVER, PLACE_SPECIAL);
		if (QuestStatus(Q_GARBUD))
			AddMonsterType(UniqMonst[UMT_GARBUD].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_ZHAR))
			AddMonsterType(UniqMonst[UMT_ZHAR].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_LTBANNER))
			AddMonsterType(UniqMonst[UMT_SNOTSPIL].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_VEIL))
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_WARLORD))
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, PLACE_UNIQUE);

		if (gbMaxPlayers != 1 && currlevel == quests[Q_SKELKING]._qlevel) {

			AddMonsterType(MT_SKING, PLACE_UNIQUE);

			nt = 0;
			for (i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

					if (currlevel >= minl && currlevel <= maxl) {
						if (MonstAvailTbl[i] & mamask) {
							skeltypes[nt++] = i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[random_(88, nt)], PLACE_SCATTER);
		}

		nt = 0;
		for (i = 0; i < NUM_MTYPES; i++) {
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if (currlevel >= minl && currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					typelist[nt++] = i;
				}
			}
		}

		while (nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000) {
			for (i = 0; i < nt;) {
				if (monsterdata[typelist[i]].mImage > 4000 - monstimgtot) {
					typelist[i] = typelist[--nt];
					continue;
				}

				i++;
			}

			if (nt != 0) {
				i = random_(88, nt);
				AddMonsterType(typelist[i], PLACE_SCATTER);
				typelist[i] = typelist[--nt];
			}
		}

	} else {
		if (setlvlnum == SL_SKELKING) {
			AddMonsterType(MT_SKING, PLACE_UNIQUE);
		}
	}
}

void InitMonsterGFX(int monst)
{
	int mtype = Monsters[monst].mtype;

	for (int anim = 0; anim < 6; anim++) {
		Monsters[monst].Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	Monsters[monst].MData = &monsterdata[mtype];
}

void ClearMVars(int i)
{
	monster[i]._mVar1 = 0;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = 0;
	monster[i]._mVar4 = 0;
	monster[i]._mVar5 = 0;
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
}

void InitMonster(int i, int rd, int mtype, int x, int y)
{
	CMonster *monst = &Monsters[mtype];

	monster[i]._mdir = rd;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._mMTidx = mtype;
	monster[i]._mmode = MM_STAND;
	monster[i].mName = monst->MData->mName;
	monster[i].MType = monst;
	monster[i].MData = monst->MData;
	monster[i]._mAnimData = monst->Anims[MA_STAND].Data[rd];
	monster[i]._mAnimDelay = monst->Anims[MA_STAND].Rate;
	monster[i]._mAnimCnt = random_(88, monster[i]._mAnimDelay - 1);
	monster[i]._mAnimLen = monst->Anims[MA_STAND].Frames;
	monster[i]._mAnimFrame = random_(88, monster[i]._mAnimLen - 1) + 1;

	if (monst->mtype == MT_DIABLO) {
#ifdef HELLFIRE
		monster[i]._mmaxhp = (random_(88, 1) + 3333) << 6;
#else
		monster[i]._mmaxhp = (random_(88, 1) + 1666) << 6;
#endif
	} else {
		monster[i]._mmaxhp = (monst->mMinHP + random_(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (gbMaxPlayers == 1) {
		monster[i]._mmaxhp >>= 1;
		if (monster[i]._mmaxhp < 64) {
			monster[i]._mmaxhp = 64;
		}
	}

	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i]._mAi = monst->MData->mAi;
	monster[i]._mint = monst->MData->mInt;
	monster[i]._mgoal = MGOAL_NORMAL;
	monster[i]._mgoalvar1 = 0;
	monster[i]._mgoalvar2 = 0;
	monster[i]._mgoalvar3 = 0;
	monster[i].field_18 = 0;
	monster[i]._pathcount = 0;
	monster[i]._mDelFlag = FALSE;
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
#ifdef HELLFIRE
	monster[i].mlid = 0;
#endif
	monster[i]._mRndSeed = GetRndSeed();
	monster[i]._mAISeed = GetRndSeed();
	monster[i].mWhoHit = 0;
	monster[i].mLevel = monst->MData->mLevel;
	monster[i].mExp = monst->MData->mExp;
	monster[i].mHit = monst->MData->mHit;
	monster[i].mMinDamage = monst->MData->mMinDamage;
	monster[i].mMaxDamage = monst->MData->mMaxDamage;
	monster[i].mHit2 = monst->MData->mHit2;
	monster[i].mMinDamage2 = monst->MData->mMinDamage2;
	monster[i].mMaxDamage2 = monst->MData->mMaxDamage2;
	monster[i].mArmorClass = monst->MData->mArmorClass;
	monster[i].mMagicRes = monst->MData->mMagicRes;
	monster[i].leader = 0;
	monster[i].leaderflag = 0;
	monster[i]._mFlags = monst->MData->mFlags;
	monster[i].mtalkmsg = 0;

	if (monster[i]._mAi == AI_GARG) {
		monster[i]._mAnimData = monst->Anims[MA_SPECIAL].Data[rd];
		monster[i]._mAnimFrame = 1;
		monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		monster[i]._mmode = MM_SATTACK;
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
#ifdef HELLFIRE
		monster[i]._mmaxhp = 3 * monster[i]._mmaxhp + ((gbMaxPlayers != 1 ? 100 : 50) << 6);
#else
		monster[i]._mmaxhp = 3 * monster[i]._mmaxhp + 64;
#endif
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 15;
		monster[i].mExp = 2 * (monster[i].mExp + 1000);
		monster[i].mHit += 85;
		monster[i].mMinDamage = 2 * (monster[i].mMinDamage + 2);
		monster[i].mMaxDamage = 2 * (monster[i].mMaxDamage + 2);
		monster[i].mHit2 += 85;
		monster[i].mMinDamage2 = 2 * (monster[i].mMinDamage2 + 2);
		monster[i].mMaxDamage2 = 2 * (monster[i].mMaxDamage2 + 2);
		monster[i].mArmorClass += 50;
	}

#ifdef HELLFIRE
	else
#endif
	    if (gnDifficulty == DIFF_HELL) {
#ifdef HELLFIRE
		monster[i]._mmaxhp = 4 * monster[i]._mmaxhp + ((gbMaxPlayers != 1 ? 200 : 100) << 6);
#else
		monster[i]._mmaxhp = 4 * monster[i]._mmaxhp + 192;
#endif
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 30;
		monster[i].mExp = 4 * (monster[i].mExp + 1000);
		monster[i].mHit += 120;
		monster[i].mMinDamage = 4 * monster[i].mMinDamage + 6;
		monster[i].mMaxDamage = 4 * monster[i].mMaxDamage + 6;
		monster[i].mHit2 += 120;
		monster[i].mMinDamage2 = 4 * monster[i].mMinDamage2 + 6;
		monster[i].mMaxDamage2 = 4 * monster[i].mMaxDamage2 + 6;
		monster[i].mArmorClass += 80;
		monster[i].mMagicRes = monst->MData->mMagicRes2;
	}
}

void ClrAllMonsters()
{
	int i;
	MonsterStruct *Monst;

	for (i = 0; i < MAXMONSTERS; i++) {
		Monst = &monster[i];
		ClearMVars(i);
		Monst->mName = "Invalid Monster";
		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->_mx = 0;
		Monst->_my = 0;
		Monst->_mfutx = 0;
		Monst->_mfuty = 0;
		Monst->_moldx = 0;
		Monst->_moldy = 0;
		Monst->_mdir = random_(89, 8);
		Monst->_mxvel = 0;
		Monst->_myvel = 0;
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random_(89, 1);
		// BUGFIX: `Monst->_menemy` may be referencing a player who already left the game, thus reading garbage data from `plr[Monst->_menemy]._pfutx`.
		Monst->_menemyx = plr[Monst->_menemy]._pfutx;
		Monst->_menemyy = plr[Monst->_menemy]._pfuty;
	}
}

static BOOL SolidLoc(int x, int y)
{
#ifndef HELLFIRE
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY) {
		return FALSE;
	}
#endif

	return nSolidTable[dPiece[x][y]];
}

BOOL MonstPlace(int xp, int yp)
{
	char f;

	if (xp < 0 || xp >= MAXDUNX
	    || yp < 0 || yp >= MAXDUNY
	    || dMonster[xp][yp] != 0
	    || dPlayer[xp][yp] != 0) {
		return FALSE;
	}

	f = dFlags[xp][yp];

	if (f & BFLAG_VISIBLE) {
		return FALSE;
	}

	if (f & BFLAG_POPULATED) {
		return FALSE;
	}

	return !SolidLoc(xp, yp);
}

#ifdef HELLFIRE
void monster_some_crypt()
{
	MonsterStruct *mon;
	int hp;

	if (currlevel == 24 && UberDiabloMonsterIndex >= 0 && UberDiabloMonsterIndex < nummonsters) {
		mon = &monster[UberDiabloMonsterIndex];
		PlayEffect(UberDiabloMonsterIndex, 2);
		quests[Q_NAKRUL]._qlog = FALSE;
		mon->mArmorClass -= 50;
		hp = mon->_mmaxhp / 2;
		mon->mMagicRes = 0;
		mon->_mhitpoints = hp;
		mon->_mmaxhp = hp;
	}
}

#endif
void PlaceMonster(int i, int mtype, int x, int y)
{
	int rd;

#ifdef HELLFIRE
	if (Monsters[mtype].mtype == MT_NAKRUL) {
		for (int j = 0; j < nummonsters; j++) {
			if (monster[j]._mMTidx == mtype) {
				return;
			}
			if (monster[j].MType->mtype == MT_NAKRUL) {
				return;
			}
		}
	}
#endif
	dMonster[x][y] = i + 1;

	rd = random_(90, 8);
	InitMonster(i, rd, mtype, x, y);
}

#ifndef SPAWN
void PlaceUniqueMonst(int uniqindex, int miniontype, int bosspacksize)
{
	int xp, yp, x, y, i;
	int uniqtype;
	int count2;
	char filestr[64];
	BOOL zharflag, done;
	UniqMonstStruct *Uniq;
	MonsterStruct *Monst;
	int count;

	Monst = &monster[nummonsters];
	count = 0;
	Uniq = &UniqMonst[uniqindex];

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE) {
		return;
	}

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (Monsters[uniqtype].mtype == UniqMonst[uniqindex].mtype) {
			break;
		}
	}

	while (1) {
		xp = random_(91, 80) + 16;
		yp = random_(91, 80) + 16;
		count2 = 0;
		for (x = xp - 3; x < xp + 3; x++) {
			for (y = yp - 3; y < yp + 3; y++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && MonstPlace(x, y)) {
					count2++;
				}
			}
		}

		if (count2 < 9) {
			count++;
			if (count < 1000) {
				continue;
			}
		}

		if (MonstPlace(xp, yp)) {
			break;
		}
	}

	if (uniqindex == UMT_SNOTSPIL) {
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if (uniqindex == UMT_WARLORD) {
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if (uniqindex == UMT_ZHAR) {
		zharflag = TRUE;
		for (i = 0; i < themeCount; i++) {
			if (i == zharlib && zharflag == TRUE) {
				zharflag = FALSE;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if (gbMaxPlayers == 1) {
		if (uniqindex == UMT_LAZURUS) {
			xp = 32;
			yp = 46;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 40;
			yp = 45;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 38;
			yp = 49;
		}
		if (uniqindex == UMT_SKELKING) {
			xp = 35;
			yp = 47;
		}
	} else {
		if (uniqindex == UMT_LAZURUS) {
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if (uniqindex == UMT_BUTCHER) {
		done = FALSE;
		for (yp = 0; yp < MAXDUNY && !done; yp++) {
			for (xp = 0; xp < MAXDUNX && !done; xp++) {
				done = dPiece[xp][yp] == 367;
			}
		}
	}

#ifdef HELLFIRE
	if (uniqindex == UMT_NAKRUL) {
		if (UberRow == 0 || UberCol == 0) {
			UberDiabloMonsterIndex = -1;
			return;
		}
		xp = UberRow - 2;
		yp = UberCol;
		UberDiabloMonsterIndex = nummonsters;
	}
#endif
	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (gbMaxPlayers == 1) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if (Monst->_mmaxhp < 64) {
			Monst->_mmaxhp = 64;
		}
	}

	Monst->_mhitpoints = Monst->_mmaxhp;
	Monst->_mAi = Uniq->mAi;
	Monst->_mint = Uniq->mint;
	Monst->mMinDamage = Uniq->mMinDamage;
	Monst->mMaxDamage = Uniq->mMaxDamage;
	Monst->mMinDamage2 = Uniq->mMinDamage;
	Monst->mMaxDamage2 = Uniq->mMaxDamage;
	Monst->mMagicRes = Uniq->mMagicRes;
	Monst->mtalkmsg = Uniq->mtalkmsg;
#ifdef HELLFIRE
	if (uniqindex == UMT_HORKDMN)
		Monst->mlid = 0;
	else
#endif
		Monst->mlid = AddLight(Monst->_mx, Monst->_my, 3);

	if (gbMaxPlayers != 1) {
		if (Monst->_mAi == AI_LAZHELP)
			Monst->mtalkmsg = 0;
#ifndef HELLFIRE
		if (Monst->_mAi != AI_LAZURUS || quests[Q_BETRAYER]._qvar1 <= 3) {
			if (Monst->mtalkmsg) {
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
#endif
	}
#ifdef HELLFIRE
	if (Monst->mtalkmsg)
#else
	else if (Monst->mtalkmsg)
#endif
		Monst->_mgoal = MGOAL_INQUIRING;

	if (gnDifficulty == DIFF_NIGHTMARE) {
#ifdef HELLFIRE
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + ((gbMaxPlayers != 1 ? 100 : 50) << 6);
#else
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
#endif
		Monst->mLevel += 15;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

#ifdef HELLFIRE
	else if (gnDifficulty == DIFF_HELL) {
#else
	if (gnDifficulty == DIFF_HELL) {
#endif
#ifdef HELLFIRE
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + ((gbMaxPlayers != 1 ? 200 : 100) << 6);
#else
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
#endif
		Monst->mLevel += 30;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	Monst->_uniqtrans = uniquetrans++;

	if (Uniq->mUnqAttr & 4) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if (Uniq->mUnqAttr & 8) {
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if (Uniq->mUnqAttr & 1) {
		PlaceGroup(miniontype, bosspacksize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
		Monst->_mAnimFrame = random_(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

static void PlaceUniques()
{
	int u, mt;
	BOOL done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != currlevel)
			continue;
		done = FALSE;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (Monsters[mt].mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UMT_GARBUD && quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_ZHAR && quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_SNOTSPIL && quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_LACHDAN && quests[Q_VEIL]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_WARLORD && quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (done)
			PlaceUniqueMonst(u, mt, 8);
	}
}

void PlaceQuestMonsters()
{
	int skeltype;
	BYTE *setp;

	if (!setlevel) {
		if (QuestStatus(Q_BUTCHER)) {
			PlaceUniqueMonst(UMT_BUTCHER, 0, 0);
		}

		if (currlevel == quests[Q_SKELKING]._qlevel && gbMaxPlayers != 1) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(Monsters[skeltype].mtype)) {
					break;
				}
			}

			PlaceUniqueMonst(UMT_SKELKING, skeltype, 30);
		}

		if (QuestStatus(Q_LTBANNER)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLOOD)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLIND)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_ANVIL)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x + 2, 2 * setpc_y + 2);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_WARLORD)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, PLACE_SCATTER);
		}
		if (QuestStatus(Q_VEIL)) {
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, PLACE_SCATTER);
		}
		if (QuestStatus(Q_ZHAR) && zharlib == -1) {
			quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if (currlevel == quests[Q_BETRAYER]._qlevel && gbMaxPlayers != 1) {
			AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, PLACE_UNIQUE);
			AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, PLACE_UNIQUE);
			PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
			PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
#ifdef HELLFIRE

		if (currlevel == 24) {
			UberDiabloMonsterIndex = -1;
			int i1;
			for (i1 = 0; i1 < nummtypes; i1++) {
				if (Monsters[i1].mtype == UniqMonst[UMT_NAKRUL].mtype)
					break;
			}

			if (i1 < nummtypes) {
				for (int i2 = 0; i2 < nummonsters; i2++) {
					if (monster[i2]._uniqtype != 0 || monster[i2]._mMTidx == i1) {
						UberDiabloMonsterIndex = i2;
						break;
					}
				}
			}
			if (UberDiabloMonsterIndex == -1)
				PlaceUniqueMonst(UMT_NAKRUL, 0, 0);
		}
#endif
	} else if (setlvlnum == SL_SKELKING) {
		PlaceUniqueMonst(UMT_SKELKING, 0, 0);
	}
}
#endif

void PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int placed, try1, try2, j;
	int xp, yp, x1, y1;

	placed = 0;

	for (try1 = 0; try1 < 10; try1++) {
		while (placed) {
			nummonsters--;
			placed--;
			dMonster[monster[nummonsters]._mx][monster[nummonsters]._my] = 0;
		}

		if (leaderf & 1) {
			int offset = random_(92, 8);
			x1 = xp = monster[leader]._mx + offset_x[offset];
			y1 = yp = monster[leader]._my + offset_y[offset];
		} else {
			do {
				x1 = xp = random_(93, 80) + 16;
				y1 = yp = random_(93, 80) + 16;
			} while (!MonstPlace(xp, yp));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (try2 = 0; j < num && try2 < 100; xp += offset_x[random_(94, 8)], yp += offset_x[random_(94, 8)]) { /// BUGFIX: `yp += offset_y`
			if (!MonstPlace(xp, yp)
			    || (dTransVal[xp][yp] != dTransVal[x1][y1])
			    || (leaderf & 2) && ((abs(xp - x1) >= 4) || (abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if (leaderf & 1) {
				monster[nummonsters]._mmaxhp *= 2;
				monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
				monster[nummonsters]._mint = monster[leader]._mint;

				if (leaderf & 2) {
					monster[nummonsters].leader = leader;
					monster[nummonsters].leaderflag = 1;
					monster[nummonsters]._mAi = monster[leader]._mAi;
				}

				if (monster[nummonsters]._mAi != AI_GARG) {
					monster[nummonsters]._mAnimData = monster[nummonsters].MType->Anims[MA_STAND].Data[monster[nummonsters]._mdir];
					monster[nummonsters]._mAnimFrame = random_(88, monster[nummonsters]._mAnimLen - 1) + 1;
					monster[nummonsters]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					monster[nummonsters]._mmode = MM_STAND;
				}
			}
			nummonsters++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if (leaderf & 2) {
		monster[leader].packsize = placed;
	}
}

#ifndef SPAWN
void LoadDiabMonsts()
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad1x, 2 * diabquad1y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad2x, 2 * diabquad2y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad3x, 2 * diabquad3y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad4x, 2 * diabquad4y);
	mem_free_dbg(lpSetPiece);
}
#endif

void InitMonsters()
{
	int na, nt;
	int i, s, t;
	int numplacemonsters;
	int mtype;
	int numscattypes;
	int scattertypes[NUM_MTYPES];

	numscattypes = 0;
	if (!setlevel) {
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
	}
#ifndef SPAWN
	if (!setlevel && currlevel == 16)
		LoadDiabMonsts();
#endif
	nt = numtrigs;
	if (currlevel == 15)
		nt = 1;
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoVision(s + trigs[i]._tx, t + trigs[i]._ty, 15, FALSE, FALSE);
		}
	}
#ifndef SPAWN
	PlaceQuestMonsters();
#endif
	if (!setlevel) {
#ifndef SPAWN
		PlaceUniques();
#endif
		na = 0;
		for (s = 16; s < 96; s++)
			for (t = 16; t < 96; t++)
				if (!SolidLoc(s, t))
					na++;
		numplacemonsters = na / 30;
		if (gbMaxPlayers != 1)
			numplacemonsters += numplacemonsters >> 1;
		if (nummonsters + numplacemonsters > MAXMONSTERS - 10)
			numplacemonsters = MAXMONSTERS - 10 - nummonsters;
		totalmonsters = nummonsters + numplacemonsters;
		for (i = 0; i < nummtypes; i++) {
			if (Monsters[i].mPlaceFlags & PLACE_SCATTER) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		while (nummonsters < totalmonsters) {
			mtype = scattertypes[random_(95, numscattypes)];
			if (currlevel == 1 || random_(95, 2) == 0)
				na = 1;
#ifdef HELLFIRE
			else if (currlevel == 2 || currlevel >= 21 && currlevel <= 24)
#else
			else if (currlevel == 2)
#endif
				na = random_(95, 2) + 2;
			else
				na = random_(95, 3) + 3;
			PlaceGroup(mtype, na, 0, 0);
		}
	}
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoUnVision(s + trigs[i]._tx, t + trigs[i]._ty, 15);
		}
	}
}

#ifndef SPAWN
void SetMapMonsters(BYTE *pMap, int startx, int starty)
{
	WORD rw, rh;
	WORD *lm;
	int i, j;
	int mtype;

	AddMonsterType(MT_GOLEM, PLACE_SPECIAL);
	// See https://github.com/diasurgical/devilutionX/pull/2822
	AddMonster(1, 0, 0, 0, FALSE); // BUGFIX: add only if setlevel is true
	AddMonster(1, 0, 0, 0, FALSE); // BUGFIX: add only if setlevel is true
	AddMonster(1, 0, 0, 0, FALSE); // BUGFIX: add only if setlevel is true
	AddMonster(1, 0, 0, 0, FALSE); // BUGFIX: add only if setlevel is true
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqMonst[UMT_BLACKJADE].mtype, PLACE_UNIQUE);
		PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
		PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
		PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
	}
	lm = (WORD *)pMap;
	rw = *lm++;
	rh = *lm++;
	lm += rw * rh;
	rw = rw << 1;
	rh = rh << 1;
	lm += rw * rh;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				mtype = AddMonsterType(MonstConvTbl[(*lm) - 1], PLACE_SPECIAL);
				PlaceMonster(nummonsters++, mtype, i + startx + 16, j + starty + 16);
			}
			lm++;
		}
	}
}
#endif

void DeleteMonster(int i)
{
	int temp;

	nummonsters--;
	temp = monstactive[nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

int AddMonster(int x, int y, int dir, int mtype, BOOL InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap)
			dMonster[x][y] = i + 1;
		InitMonster(i, dir, mtype, x, y);
		return i;
	}

	return -1;
}

BOOL PosOkMonst(int i, int x, int y)
{
#ifdef HELLFIRE
	int oi;
	BOOL ret;

	ret = !SolidLoc(x, y) && dPlayer[x][y] == 0 && dMonster[x][y] == 0;
	oi = dObject[x][y];
	if (ret && oi != 0) {
		oi = oi > 0 ? oi - 1 : -(oi + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret)
		ret = monster_posok(i, x, y);
#else
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y) && dPlayer[x][y] == 0 && dMonster[x][y] == 0;
	if (ret && dObject[x][y] != 0) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	//if (ret && dMissile[x][y] != 0 && i >= 0) {
	//	// BUGFIX: case with multiple missiles being present on (x, y)-coordinate not handled.
	//	mi = dMissile[x][y];
	//	if (mi > 0) {
	//		if (missile[mi]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1'
	//			fire = TRUE;
	//		} else {
	//			for (j = 0; j < nummissiles; j++) {
	//				if (missile[missileactive[j]]._mitype == MIS_FIREWALL) // BUGFIX: Check missile x/y
	//					fire = TRUE;
	//			}
	//		}
	//	}
	//	if (fire && (!(monster[i].mMagicRes & IMMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
	//		ret = FALSE;
	//}
#endif

	return ret;
}

//BOOL PosOkMonst2(int i, int x, int y)
//{
//	int oi, mi, j;
//#ifdef HELLFIRE
//	BOOL ret;
//
//	oi = dObject[x][y];
//	ret = !SolidLoc(x, y);
//	if (ret && oi != 0) {
//		oi = oi > 0 ? oi - 1 : -(oi + 1);
//		if (object[oi]._oSolidFlag)
//			ret = FALSE;
//	}
//
//	if (ret)
//		ret = monster_posok(i, x, y);
//#else
//	BOOL ret, fire;
//
//	fire = FALSE;
//	ret = !SolidLoc(x, y);
//	if (ret && dObject[x][y] != 0) {
//		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
//		if (object[oi]._oSolidFlag)
//			ret = FALSE;
//	}
//
//	if (ret && dMissile[x][y] != 0 && i >= 0) {
//		mi = dMissile[x][y];
//		// BUGFIX: case with multiple missiles being present on (x, y)-coordinate not handled.
//		if (mi > 0) {
//			if (missile[mi]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1'
//				fire = TRUE;
//			} else {
//				for (j = 0; j < nummissiles; j++) {
//					if (missile[missileactive[j]]._mitype == MIS_FIREWALL) // BUGFIX: Check missile x/y
//						fire = TRUE;
//				}
//			}
//		}
//		if (fire && (!(monster[i].mMagicRes & IMMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
//			ret = FALSE;
//	}
//#endif
//
//	return ret;
//}
//
//BOOL PosOkMonst3(int i, int x, int y)
//{
//	int j, oi, objtype, mi;
//#ifdef HELLFIRE
//	BOOL ret;
//	DIABOOL isdoor;
//
//	ret = TRUE;
//	isdoor = FALSE;
//
//	oi = dObject[x][y];
//	if (ret && oi != 0) {
//		oi = oi > 0 ? oi - 1 : -(oi + 1);
//		objtype = object[oi]._otype;
//		isdoor = objtype == OBJ_L1LDOOR || objtype == OBJ_L1RDOOR
//		    || objtype == OBJ_L2LDOOR || objtype == OBJ_L2RDOOR
//		    || objtype == OBJ_L3LDOOR || objtype == OBJ_L3RDOOR;
//		if (object[oi]._oSolidFlag && !isdoor) {
//			ret = FALSE;
//		}
//	}
//	if (ret) {
//		ret = (!SolidLoc(x, y) || isdoor) && dPlayer[x][y] == 0 && dMonster[x][y] == 0;
//	}
//	if (ret)
//		ret = monster_posok(i, x, y);
//#else
//	BOOL ret, fire, isdoor;
//
//	fire = FALSE;
//	ret = TRUE;
//	isdoor = FALSE;
//
//	if (ret && dObject[x][y] != 0) {
//		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
//		objtype = object[oi]._otype;
//		isdoor = objtype == OBJ_L1LDOOR || objtype == OBJ_L1RDOOR
//		    || objtype == OBJ_L2LDOOR || objtype == OBJ_L2RDOOR
//		    || objtype == OBJ_L3LDOOR || objtype == OBJ_L3RDOOR;
//		if (object[oi]._oSolidFlag && !isdoor) {
//			ret = FALSE;
//		}
//	}
//	if (ret) {
//		ret = (!SolidLoc(x, y) || isdoor) && dPlayer[x][y] == 0 && dMonster[x][y] == 0;
//	}
//	if (ret && dMissile[x][y] != 0 && i >= 0) {
//		mi = dMissile[x][y];
//		// BUGFIX: case with multiple missiles being present on (x, y)-coordinate not handled.
//		if (mi > 0) {
//			if (missile[mi]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1'
//				fire = TRUE;
//			} else {
//				for (j = 0; j < nummissiles; j++) {
//					if (missile[missileactive[j]]._mitype == MIS_FIREWALL) // BUGFIX: Check missile x/y
//						fire = TRUE;
//				}
//			}
//		}
//		if (fire && (!(monster[i].mMagicRes & IMMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
//			ret = FALSE;
//	}
//#endif
//
//	return ret;
//}

BOOL IsSkel(int mt)
{
	return mt >= MT_WSKELAX && mt <= MT_XSKELAX
	    || mt >= MT_WSKELBW && mt <= MT_XSKELBW
	    || mt >= MT_WSKELSD && mt <= MT_XSKELSD;
}

BOOL IsGoat(int mt)
{
	return mt >= MT_NGOATMC && mt <= MT_GGOATMC
	    || mt >= MT_NGOATBW && mt <= MT_GGOATBW;
}

//int M_SpawnSkel(int x, int y, int dir)
//{
//	int i, j, skeltypes, skel;
//
//	j = 0;
//	for (i = 0; i < nummtypes; i++) {
//		if (IsSkel(Monsters[i].mtype))
//			j++;
//	}
//
//	if (j) {
//		skeltypes = random_(136, j);
//		j = 0;
//		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
//			if (IsSkel(Monsters[i].mtype))
//				j++;
//		}
//		skel = AddMonster(x, y, dir, i - 1, TRUE);
//		if (skel != -1)
//			M_StartSpStand(skel, dir);
//
//		return skel;
//	}
//
//	return -1;
//}

void ActivateSpawn(int i, int x, int y, int dir)
{
	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
}

BOOL SpawnSkeleton(int ii, int x, int y)
{
	int dx, dy, xx, yy, dir, j, k, rs;
	BOOL savail;
	int monstok[3][3];

	if (ii == -1)
		return FALSE;

	if (PosOkMonst(-1, x, y)) {
		dir = DIR_S;
		ActivateSpawn(ii, x, y, dir);
		return TRUE;
	}

	savail = FALSE;
	yy = 0;
	for (j = y - 1; j <= y + 1; j++) {
		xx = 0;
		for (k = x - 1; k <= x + 1; k++) {
			monstok[xx][yy] = PosOkMonst(-1, k, j);
			savail |= monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return FALSE;
	}

	rs = random_(137, 15) + 1;
	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (monstok[xx][yy])
			rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3)
					yy = 0;
			}
		}
	}

	dx = x - 1 + xx;
	dy = y - 1 + yy;
	dir = DIR_S;
	ActivateSpawn(ii, dx, dy, dir);

	return TRUE;
}

int PreSpawnSkeleton()
{
	int i, j, skeltypes, skel;

	j = 0;

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j) {
		skeltypes = random_(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(0, 0, 0, i - 1, FALSE);
		//if (skel != -1)
		//	M_StartStand(skel, 0);

		return skel;
	}

	return -1;
}
