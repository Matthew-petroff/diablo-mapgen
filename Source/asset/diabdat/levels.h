#pragma once

#include "../cacheconfig.h"
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

	// This directory is only used during town generation, which is currently
	// not useful for map generation. This member is only defined to remove the
	// usage of LoadFileInMem.
	levels::TownData townData;

	static Levels LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();
};

}  // namespace diabdat
}  // namespace asset
