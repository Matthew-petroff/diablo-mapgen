#include "level.h"

#include <cstdint>
#include <stdio.h>

#include "../types.h"
#include "engine.h"
#include "gendung.h"
#include "monstdat.h"
#include "monster.h"
#include "objdat.h"
#include "objects.h"

inline void WriteLE16(FILE *out, uint16_t val)
{
	fwrite(&val, 1, 2, out);
}

void ExportDun(int seed)
{
	char fileName[32];
	sprintf(fileName, "%d-%d-%d.dun", seed, currlevel, glSeedTbl[currlevel]);
	FILE *dunFile = fopen(fileName, "wb");

	WriteLE16(dunFile, DMAXX);
	WriteLE16(dunFile, DMAXY);

	/** Tiles. */
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			WriteLE16(dunFile, dungeon[x][y]);
		}
	}

	/** Padding */
	for (int y = 16; y < MAXDUNY - 16; y++) {
		for (int x = 16; x < MAXDUNX - 16; x++) {
			WriteLE16(dunFile, 0);
		}
	}

	/** Monsters */
	for (int y = 16; y < MAXDUNY - 16; y++) {
		for (int x = 16; x < MAXDUNX - 16; x++) {
			uint16_t monsterId = 0;
			if (dMonster[x][y] > 0) {
				for (int i = 0; i < 128; i++) {
					if (MonstConvTbl[i] == monster[dMonster[x][y] - 1].MType->mtype) {
						monsterId = i + 1;
						break;
					}
				}
			}
			WriteLE16(dunFile, monsterId);
		}
	}

	/** Objects */
	for (int y = 16; y < MAXDUNY - 16; y++) {
		for (int x = 16; x < MAXDUNX - 16; x++) {
			uint16_t objectId = 0;
			if (dObject[x][y] > 0) {
				for (int i = 0; i < 147; i++) {
					if (ObjTypeConv[i] == object[dObject[x][y] - 1]._otype) {
						objectId = i;
						break;
					}
				}
			}
			WriteLE16(dunFile, objectId);
		}
	}

	/** Transparency */
	for (int y = 16; y < MAXDUNY - 16; y++) {
		for (int x = 16; x < MAXDUNX - 16; x++) {
			WriteLE16(dunFile, dTransVal[x][y]);
		}
	}
	fclose(dunFile);
}
