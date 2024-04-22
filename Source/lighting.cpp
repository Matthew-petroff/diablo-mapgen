/**
 * @file lighting.cpp
 *
 * Implementation of light and vision.
 */
#include "all.h"

LightListStruct VisionList[MAXVISION];
BYTE lightactive[MAXLIGHTS];
LightListStruct LightList[MAXLIGHTS];
int numlights;
BYTE lightradius[16][128];
BOOL dovision;
int numvision;
BOOL dolighting;
BYTE lightblock[64][16][16];
int visionid;
BYTE *pLightTbl;
BOOL lightflag;

/** vCrawlTable specifies the X- Y-coordinate offsets of lighting visions. */
BYTE vCrawlTable[23][30] = {
	{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0 },
	{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15, 1 },
	{ 1, 0, 2, 0, 3, 0, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 2, 13, 2, 14, 2, 15, 2 },
	{ 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 2, 9, 2, 10, 2, 11, 2, 12, 2, 13, 3, 14, 3, 15, 3 },
	{ 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 2, 7, 2, 8, 2, 9, 3, 10, 3, 11, 3, 12, 3, 13, 4, 14, 4, 0, 0 },
	{ 1, 0, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 3, 8, 3, 9, 3, 10, 4, 11, 4, 12, 4, 13, 5, 14, 5, 0, 0 },
	{ 1, 0, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 5, 12, 5, 13, 6, 14, 6, 0, 0 },
	{ 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 4, 8, 4, 9, 5, 10, 5, 11, 6, 12, 6, 13, 7, 0, 0, 0, 0 },
	{ 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 4, 7, 4, 8, 5, 9, 6, 10, 6, 11, 7, 12, 7, 12, 8, 13, 8, 0, 0 },
	{ 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 5, 7, 5, 8, 6, 9, 7, 10, 7, 10, 8, 11, 8, 12, 9, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 8, 10, 9, 11, 9, 11, 10, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 9, 11, 10, 11, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 7, 10, 8, 10, 8, 11, 9, 12, 0, 0, 0, 0 },
	{ 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 4, 7, 5, 8, 6, 9, 6, 10, 7, 11, 7, 12, 8, 12, 8, 13, 0, 0 },
	{ 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 3, 6, 4, 7, 4, 8, 5, 9, 5, 10, 6, 11, 6, 12, 7, 13, 0, 0, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 3, 8, 4, 9, 4, 10, 5, 11, 5, 12, 6, 13, 6, 14, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 4, 12, 5, 13, 5, 14, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 2, 8, 3, 9, 3, 10, 3, 11, 3, 12, 4, 13, 4, 14, 0, 0 },
	{ 0, 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 2, 8, 2, 9, 2, 10, 2, 11, 2, 12, 3, 13, 3, 14, 3, 15 },
	{ 0, 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 2, 12, 2, 13, 2, 14, 2, 15 },
	{ 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15 },
	{ 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15 }
};
/** unused */
BYTE byte_49463C[18][18] = {
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2 }
};

/** RadiusAdj maps from vCrawlTable index to lighting vision radius adjustment. */
BYTE RadiusAdj[23] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0 };

void RotateRadius(int *x, int *y, int *dx, int *dy, int *lx, int *ly, int *bx, int *by)
{
	int swap;

	*bx = 0;
	*by = 0;

	swap = *dx;
	*dx = 7 - *dy;
	*dy = swap;
	swap = *lx;
	*lx = 7 - *ly;
	*ly = swap;

	*x = *dx - *lx;
	*y = *dy - *ly;

	if (*x < 0) {
		*x += 8;
		*bx = 1;
	}
	if (*y < 0) {
		*y += 8;
		*by = 1;
	}
}

void DoLighting(int nXPos, int nYPos, int nRadius, int Lnum)
{
}

void DoUnLight(int nXPos, int nYPos, int nRadius)
{
	int x, y, min_x, min_y, max_x, max_y;

	nRadius++;
	min_y = nYPos - nRadius;
	max_y = nYPos + nRadius;
	min_x = nXPos - nRadius;
	max_x = nXPos + nRadius;

	if (min_y < 0) {
		min_y = 0;
	}
	if (max_y > MAXDUNY) {
		max_y = MAXDUNY;
	}
	if (min_x < 0) {
		min_x = 0;
	}
	if (max_x > MAXDUNX) {
		max_x = MAXDUNX;
	}

	for (y = min_y; y < max_y; y++) {
		for (x = min_x; x < max_x; x++) {
#ifndef HELLFIRE
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY)
#endif
				dLight[x][y] = dPreLight[x][y];
		}
	}
}

void DoUnVision(int nXPos, int nYPos, int nRadius)
{
	int i, j, x1, y1, x2, y2;

	nRadius++;
	y1 = nYPos - nRadius;
	y2 = nYPos + nRadius;
	x1 = nXPos - nRadius;
	x2 = nXPos + nRadius;

	if (y1 < 0) {
		y1 = 0;
	}
	if (y2 > MAXDUNY) {
		y2 = MAXDUNY;
	}
	if (x1 < 0) {
		x1 = 0;
	}
	if (x2 > MAXDUNX) {
		x2 = MAXDUNX;
	}

	for (i = x1; i < x2; i++) {
		for (j = y1; j < y2; j++) {
			dFlags[i][j] &= ~(BFLAG_VISIBLE | BFLAG_LIT);
		}
	}
}

void DoVision(int nXPos, int nYPos, int nRadius, BOOL doautomap, BOOL visible)
{
	BOOL nBlockerFlag;
	int nCrawlX, nCrawlY, nLineLen, nTrans;
	int j, k, v, x1adj, x2adj, y1adj, y2adj;

	if (nXPos >= 0 && nXPos <= MAXDUNX && nYPos >= 0 && nYPos <= MAXDUNY) { // BUGFIX < MAXDUNX/MAXDUNY or OOB
		if (doautomap) {
			if (dFlags[nXPos][nYPos] >= 0) {
				SetAutomapView(nXPos, nXPos); // BUGFIX - second argument should be nYPos
			}
			dFlags[nXPos][nYPos] |= BFLAG_EXPLORED;
		}
		if (visible) {
			dFlags[nXPos][nYPos] |= BFLAG_LIT;
		}
		dFlags[nXPos][nYPos] |= BFLAG_VISIBLE;
	}

	for (v = 0; v < 4; v++) {
		for (j = 0; j < 23; j++) {
			nBlockerFlag = FALSE;
			nLineLen = 2 * (nRadius - RadiusAdj[j]);
			for (k = 0; k < nLineLen && !nBlockerFlag; k += 2) {
				x1adj = 0;
				x2adj = 0;
				y1adj = 0;
				y2adj = 0;
				switch (v) {
				case 0:
					nCrawlX = nXPos + vCrawlTable[j][k];
					nCrawlY = nYPos + vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						x1adj = -1;
						y2adj = -1;
					}
					break;
				case 1:
					nCrawlX = nXPos - vCrawlTable[j][k];
					nCrawlY = nYPos - vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						y1adj = 1;
						x2adj = 1;
					}
					break;
				case 2:
					nCrawlX = nXPos + vCrawlTable[j][k];
					nCrawlY = nYPos - vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						x1adj = -1;
						y2adj = 1;
					}
					break;
				case 3:
					nCrawlX = nXPos - vCrawlTable[j][k];
					nCrawlY = nYPos + vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						y1adj = -1;
						x2adj = 1;
					}
					break;
				}
				if (nCrawlX >= 0 && nCrawlX <= MAXDUNX && nCrawlY >= 0 && nCrawlY <= MAXDUNY) {
					nBlockerFlag = nBlockTable[dPiece[nCrawlX][nCrawlY]];
					if (!nBlockTable[dPiece[x1adj + nCrawlX][y1adj + nCrawlY]]
					    || !nBlockTable[dPiece[x2adj + nCrawlX][y2adj + nCrawlY]]) {
						if (doautomap) {
							if (dFlags[nCrawlX][nCrawlY] >= 0) {
								SetAutomapView(nCrawlX, nCrawlY);
							}
							dFlags[nCrawlX][nCrawlY] |= BFLAG_EXPLORED;
						}
						if (visible) {
							dFlags[nCrawlX][nCrawlY] |= BFLAG_LIT;
						}
						dFlags[nCrawlX][nCrawlY] |= BFLAG_VISIBLE;
						if (!nBlockerFlag) {
							nTrans = dTransVal[nCrawlX][nCrawlY];
							if (nTrans != 0) {
								TransList[nTrans] = TRUE;
							}
						}
					}
				}
			}
		}
	}
}

#ifdef _DEBUG
void ToggleLighting_2()
{
	int i;

	if (lightflag) {
		memset(dLight, 0, sizeof(dLight));
	} else {
		memset(dLight, 0, sizeof(dLight));
		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].plractive && plr[i].plrlevel == currlevel) {
				DoLighting(plr[i]._px, plr[i]._py, plr[i]._pLightRad, -1);
			}
		}
	}
}

void ToggleLighting()
{
	int i;

	lightflag ^= TRUE;

	if (lightflag) {
		memset(dLight, 0, sizeof(dLight));
	} else {
		memcpy(dLight, dPreLight, sizeof(dLight));
		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].plractive && plr[i].plrlevel == currlevel) {
				DoLighting(plr[i]._px, plr[i]._py, plr[i]._pLightRad, -1);
			}
		}
	}
}
#endif

int AddLight(int x, int y, int r)
{
	return -1;
}

void AddUnLight(int i)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._ldel = TRUE;
	dolighting = TRUE;
}

void ChangeLightRadius(int i, int r)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

void ChangeLightXY(int i, int x, int y)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lx = x;
	LightList[i]._ly = y;
	dolighting = TRUE;
}

void ChangeLightOff(int i, int x, int y)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._xoff = x;
	LightList[i]._yoff = y;
	dolighting = TRUE;
}

void ChangeLight(int i, int x, int y, int r)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._lunflag = TRUE;
	LightList[i]._lunx = LightList[i]._lx;
	LightList[i]._luny = LightList[i]._ly;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lx = x;
	LightList[i]._ly = y;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

void ProcessLightList()
{
	int i, j;
	BYTE temp;

	if (lightflag) {
		return;
	}

	if (dolighting) {
		for (i = 0; i < numlights; i++) {
			j = lightactive[i];
			if (LightList[j]._ldel) {
				DoUnLight(LightList[j]._lx, LightList[j]._ly, LightList[j]._lradius);
			}
			if (LightList[j]._lunflag) {
				DoUnLight(LightList[j]._lunx, LightList[j]._luny, LightList[j]._lunr);
				LightList[j]._lunflag = FALSE;
			}
		}
		for (i = 0; i < numlights; i++) {
			j = lightactive[i];
			if (!LightList[j]._ldel) {
				DoLighting(LightList[j]._lx, LightList[j]._ly, LightList[j]._lradius, j);
			}
		}
		i = 0;
		while (i < numlights) {
			if (LightList[lightactive[i]]._ldel) {
				numlights--;
				temp = lightactive[numlights];
				lightactive[numlights] = lightactive[i];
				lightactive[i] = temp;
			} else {
				i++;
			}
		}
	}

	dolighting = FALSE;
}

void SavePreLighting()
{
	memcpy(dPreLight, dLight, sizeof(dPreLight));
}

void InitVision()
{
	memset(TransList, 0, sizeof(TransList));

}

void ChangeVisionRadius(int id, int r)
{
	int i;

	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lunx = VisionList[i]._lx;
			VisionList[i]._luny = VisionList[i]._ly;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._lradius = r;
			dovision = TRUE;
		}
	}
}

void ChangeVisionXY(int id, int x, int y)
{
}

void ProcessVisionList()
{
	int i;
	BOOL delflag;

	if (dovision) {
		for (i = 0; i < numvision; i++) {
			if (VisionList[i]._ldel) {
				DoUnVision(VisionList[i]._lx, VisionList[i]._ly, VisionList[i]._lradius);
			}
			if (VisionList[i]._lunflag) {
				DoUnVision(VisionList[i]._lunx, VisionList[i]._luny, VisionList[i]._lunr);
				VisionList[i]._lunflag = FALSE;
			}
		}
		for (i = 0; i < TransVal; i++) {
			TransList[i] = FALSE;
		}
		for (i = 0; i < numvision; i++) {
			if (!VisionList[i]._ldel) {
				DoVision(
				    VisionList[i]._lx,
				    VisionList[i]._ly,
				    VisionList[i]._lradius,
				    VisionList[i]._lflags & 1,
				    VisionList[i]._lflags & 1);
			}
		}
		do {
			delflag = FALSE;
			for (i = 0; i < numvision; i++) {
				if (VisionList[i]._ldel) {
					numvision--;
					if (numvision > 0 && i != numvision) {
						VisionList[i] = VisionList[numvision];
					}
					delflag = TRUE;
				}
			}
		} while (delflag);
	}

	dovision = FALSE;
}

void lighting_color_cycling()
{
	int i, j, l;
	BYTE col;
	BYTE *tbl;

	l = light4flag ? 4 : 16;

	if (leveltype != DTYPE_HELL) {
		return;
	}

	tbl = pLightTbl;

	for (j = 0; j < l; j++) {
		tbl++;
		col = *tbl;
		for (i = 0; i < 30; i++) {
			tbl[0] = tbl[1];
			tbl++;
		}
		*tbl = col;
		tbl += 225;
	}
}
