#pragma once

#include "../funkMapGen.h"

class ScannerItem : public Scanner {
public:
	bool skipLevel(int level) override;
	bool levelMatches(std::optional<uint32_t> levelSeed) override;
};

bool LocateItem(bool searchMonsters = true);
bool LocateItems(int type, bool searchMonsters = true, bool clearItems = true);
