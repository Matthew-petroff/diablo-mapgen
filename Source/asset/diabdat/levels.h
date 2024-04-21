#pragma once

#include "levels/l1data.h"
#include "levels/l2data.h"
#include "levels/l3data.h"
#include "levels/l4data.h"
#include "levels/towndata.h"

namespace asset {
namespace diabdat {

struct Levels {
	levels::L1Data l1data;
	levels::L2Data l2data;
	levels::L3Data l3data;
	levels::L4Data l4data;
	levels::TownData townData;

	static Levels LoadDirectory();
	void UnloadDirectory();
};

}  // namespace diabdat
}  // namespace asset
