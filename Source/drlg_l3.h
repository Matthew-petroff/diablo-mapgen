/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#ifndef __DRLG_L3_H__
#define __DRLG_L3_H__

std::optional<uint32_t> CreateL3Dungeon(DWORD rseed, int entry, DungeonMode mode);
void LoadL3Dungeon(const BYTE *pLevelMap, int vx, int vy);
void LoadPreL3Dungeon(const BYTE *pLevelMap, int vx, int vy);

#endif /* __DRLG_L3_H__ */
