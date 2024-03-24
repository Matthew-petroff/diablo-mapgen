#pragma once

#include "../funkMapGen.h"

class ScannerPuzzler : public Scanner {
public:
	bool skipLevel(int level) override;
	bool levelMatches(int levelSeed) override;
};
