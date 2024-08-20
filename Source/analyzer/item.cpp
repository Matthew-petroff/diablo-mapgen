/**
 * @file analyzer/item.cpp
 *
 * Implementation of scanner for locating items within dungeon levels.
 */

#include "item.h"

#include <iostream>

#include "../items.h"
#include "../monster.h"
#include "../objects.h"

/*
 * @brief Converts an object in the dungeon level to the opened/destroyed state to force generate items.
 */
static void SpawnObjectItem(int oid)
{
	auto &objectInfo = object[oid];

	switch (objectInfo._otype) {
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		SetRndSeed(objectInfo._oRndSeed);
		if (setlevel) {
			for (int j = 0; j < objectInfo._oVar1; j++) {
				CreateRndItem(objectInfo._ox, objectInfo._oy, TRUE, TRUE, FALSE);
			}
		} else {
			for (int j = 0; j < objectInfo._oVar1; j++) {
				if (objectInfo._oVar2 != 0)
					CreateRndItem(objectInfo._ox, objectInfo._oy, FALSE, TRUE, FALSE);
				else
					CreateRndUseful(0, objectInfo._ox, objectInfo._oy, TRUE);
			}
		}
		break;
	case OBJ_SARC:
		SetRndSeed(objectInfo._oRndSeed);
		if (objectInfo._oVar1 <= 2)
			CreateRndItem(objectInfo._ox, objectInfo._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_DECAP:
		SetRndSeed(objectInfo._oRndSeed);
		CreateRndItem(objectInfo._ox, objectInfo._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_BARREL:
		SetRndSeed(objectInfo._oRndSeed);
		if (objectInfo._oVar2 <= 1) {
			if (objectInfo._oVar3 == 0)
				CreateRndUseful(0, objectInfo._ox, objectInfo._oy, TRUE);
			else
				CreateRndItem(objectInfo._ox, objectInfo._oy, FALSE, TRUE, FALSE);
		}
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		SetRndSeed(objectInfo._oRndSeed);
		if (random_(161, 5) != 0)
			CreateTypeItem(objectInfo._ox, objectInfo._oy, FALSE, ITYPE_MISC, IMISC_SCROLL, TRUE, FALSE);
		else
			CreateTypeItem(objectInfo._ox, objectInfo._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		SetRndSeed(objectInfo._oRndSeed);
		CreateTypeItem(objectInfo._ox, objectInfo._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR: {
		SetRndSeed(objectInfo._oRndSeed);
		BOOL uniqueRnd = random_(0, 2);
		if (currlevel <= 5) {
			CreateTypeItem(objectInfo._ox, objectInfo._oy, TRUE, ITYPE_LARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 6 && currlevel <= 9) {
			CreateTypeItem(objectInfo._ox, objectInfo._oy, uniqueRnd, ITYPE_MARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 10 && currlevel <= 12) {
			CreateTypeItem(objectInfo._ox, objectInfo._oy, FALSE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 13 && currlevel <= 16) {
			CreateTypeItem(objectInfo._ox, objectInfo._oy, TRUE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		}
	} break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK: {
		SetRndSeed(objectInfo._oRndSeed);
		int weaponType = ITYPE_MISC;

		switch (random_(0, 4) + ITYPE_SWORD) {
		case ITYPE_SWORD:
			weaponType = ITYPE_SWORD;
			break;
		case ITYPE_AXE:
			weaponType = ITYPE_AXE;
			break;
		case ITYPE_BOW:
			weaponType = ITYPE_BOW;
			break;
		case ITYPE_MACE:
			weaponType = ITYPE_MACE;
			break;
		}

		if (leveltype > 1)
			CreateTypeItem(objectInfo._ox, objectInfo._oy, TRUE, weaponType, IMISC_NONE, TRUE, FALSE);
		else
			CreateTypeItem(objectInfo._ox, objectInfo._oy, FALSE, weaponType, IMISC_NONE, TRUE, FALSE);
	} break;
	}
}

/*
 * @brief Force generates all items in the dungeon level.
 */
static void SpawnAllItems()
{
	// Spawn monster drops
	MonsterItems = numitems;
	for (int i = 0; i < nummonsters; i++) {
		int monsterId = monstactive[i];
		auto &monsterInfo = monster[monsterId];

		if (monsterInfo.MType->mtype == MT_GOLEM)
			continue;
		SetRndSeed(monsterInfo._mRndSeed);
		SpawnItem(monsterId, monsterInfo._mx, monsterInfo._my, TRUE);
	}

	// Spawn object drops
	ObjectItems = numitems;
	for (int i = 0; i < nobjects; i++) {
		int objectId = objectactive[i];
		SpawnObjectItem(objectId);
	}
}

bool ScannerItem::skipLevel(int level)
{
	if (*Config.target == 0)
		return false;
	return level != *Config.target;
}

/*
 * @brief Searches for the target item in the dungeon level.
 */
bool LocateItem()
{
	SpawnAllItems();

	POI = { -1, -1 };

	for (int i = 0; i < numitems; i++) {
		int itemIndex = itemactive[i];
		auto &currentItem = item[itemIndex];

		if (Config.targetStr == currentItem._iIName) {
			POI = { currentItem._ix, currentItem._iy };
			return true;
		}
	}

	return false;
}

bool ScannerItem::levelMatches(std::optional<uint32_t> levelSeed)
{
	if (!LocateItem())
		return false; // Target item not found

	if (Config.verbose) {
		// Monster information
		std::cerr << "Monster Count: " << nummonsters << std::endl;

		for (int i = 0; i < nummonsters; i++) {
			const auto &monsterInfo = monster[monstactive[i]];

			std::cerr << "Monster " << i << ": " << monsterInfo.mName
			          << " (Seed: " << monsterInfo._mRndSeed << ")" << std::endl;
		}

		// Object information
		std::cerr << "\nObject Count: " << nobjects << std::endl;

		for (int i = 0; i < nobjects; i++) {
			auto objectId = objectactive[i];
			const auto &objectInfo = object[objectId];
			char objectStr[50];

			GetObjectStr(objectId, objectStr);
			std::cerr << "Object " << i << ": " << objectStr
			          << " (Seed: " << objectInfo._oRndSeed << ")" << std::endl;
		}

		// Item information
		std::cerr << "\nItem Count: " << numitems << std::endl;

		for (int i = 0; i < numitems; i++) {
			std::string prefix;

			if (i >= ObjectItems) {
				prefix = "Object ";
			} else if (i >= MonsterItems) {
				prefix = "Monster ";
			}
			const auto &itemInfo = item[itemactive[i]];
			std::cerr << prefix << "Item " << i << ": " << itemInfo._iIName
			          << " (Seed: " << itemInfo._iSeed << ")" << std::endl;
		}
	}

	std::cout << sgGameInitInfo.dwSeed << std::endl;

	return true;
}
