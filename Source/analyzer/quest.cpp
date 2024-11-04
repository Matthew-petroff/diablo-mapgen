#include "quest.h"

#include <iostream>

#include "../../types.h"

#include "../funkMapGen.h"
#include "../quests.h"

bool ScannerQuest::skipSeed()
{
	if (quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL || quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL) {
		return true;
	}

	std::cout << sgGameInitInfo.dwSeed << std::endl;

	return true;
}
