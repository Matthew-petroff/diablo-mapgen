/**
 * @file trigs.cpp
 *
 * Implementation of functionality for triggering events when the player enters an area.
 */
#include "all.h"

BOOL townwarps[3];
BOOL trigflag;
int numtrigs;
TriggerStruct trigs[MAXTRIGGERS];
int TWarpFrom;

/** Specifies the dungeon piece IDs which constitute stairways leading down to the cathedral from town. */
int TownDownList[] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down to the catacombs from town. */
int TownWarp1List[] = { 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1181, 1183, 1185, -1 };
#ifdef HELLFIRE
int TownCryptList[] = { 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, -1 };
int TownHiveList[] = { 1307, 1308, 1309, 1310, -1 };
#endif
/** Specifies the dungeon piece IDs which constitute stairways leading up from the cathedral. */
int L1UpList[] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the cathedral. */
int L1DownList[] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from the catacombs. */
int L2UpList[] = { 266, 267, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the catacombs. */
int L2DownList[] = { 269, 270, 271, 272, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from the catacombs. */
int L2TWarpUpList[] = { 558, 559, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from the caves. */
int L3UpList[] = { 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the caves. */
int L3DownList[] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from the caves. */
int L3TWarpUpList[] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from hell. */
int L4UpList[] = { 82, 83, 90, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from hell. */
int L4DownList[] = { 120, 130, 131, 132, 133, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from hell. */
int L4TWarpUpList[] = { 421, 422, 429, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down to Diablo from hell. */
int L4PentaList[] = { 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, -1 };
#ifdef HELLFIRE
int L5TWarpUpList[] = { 172, 173, 174, 175, 176, 177, 178, 179, 184, -1 };
int L5UpList[] = { 149, 150, 151, 152, 153, 154, 155, 157, 158, 159, -1 };
int L5DownList[] = { 125, 126, 129, 131, 132, 135, 136, 140, 142, -1 };
int L6TWarpUpList[] = { 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, -1 };
int L6UpList[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, -1 };
int L6DownList[] = { 57, 58, 59, 60, 61, 62, 63, 64, -1 };
#endif

void InitNoTriggers()
{
	numtrigs = 0;
	trigflag = FALSE;
}

void InitL1Triggers()
{
	int i, j;

	numtrigs = 0;
#ifdef HELLFIRE
	if (currlevel < 17) {
#endif
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 129) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 115) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
#ifdef HELLFIRE
	} else {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 184) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					trigs[numtrigs]._tlvl = 0;
					numtrigs++;
				}
				if (dPiece[i][j] == 158) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 126) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
	}
#endif
	trigflag = FALSE;
}

void InitL2Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 267 && (i != quests[Q_SCHAMB]._qtx || j != quests[Q_SCHAMB]._qty)) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 559) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (dPiece[i][j] == 271) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitL3Triggers()
{
	int i, j;

#ifdef HELLFIRE
	if (currlevel < 17) {
#endif
		numtrigs = 0;
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 171) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 168) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 549) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					numtrigs++;
				}
			}
		}
#ifdef HELLFIRE
	} else {
		numtrigs = 0;
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 66) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 63) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 80) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					numtrigs++;
				}
			}
		}
	}
#endif
	trigflag = FALSE;
}

void InitL4Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 83) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 422) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (dPiece[i][j] == 120) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
			if (dPiece[i][j] == 336 || dPiece[i][j] == 370) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitSKingTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 82;
	trigs[0]._ty = 42;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitSChambTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 70;
	trigs[0]._ty = 39;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitPWaterTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 30;
	trigs[0]._ty = 83;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitVPTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 35;
	trigs[0]._ty = 32;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}
