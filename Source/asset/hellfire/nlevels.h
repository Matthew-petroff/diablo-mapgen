#pragma once

#include "../cacheconfig.h"
#include "nlevels/l5data.h"
#include "nlevels/l6data.h"
#include "nlevels/towndata.h"

namespace asset {
namespace hellfire {

struct NLevels {
	nlevels::L5Data l5data;
	nlevels::L6Data l6data;

	// This directory is only used during town generation, which is currently
	// not useful for map generation. This member is only defined to remove the
	// usage of LoadFileInMem.
	nlevels::TownData townData;

	static NLevels LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();
};

}  // namespace hellfire
}  // namespace asset
