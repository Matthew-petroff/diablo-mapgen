/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#ifndef __DRLG_L2_H__
#define __DRLG_L2_H__

void LoadL2Dungeon(const BYTE *pLevelMap, int vx, int vy);
void LoadPreL2Dungeon(const BYTE *pLevelMap, int vx, int vy);
std::optional<uint32_t> CreateL2Dungeon(DWORD rseed, int entry, DungeonMode mode);
#endif /* __DRLG_L2_H__ */
