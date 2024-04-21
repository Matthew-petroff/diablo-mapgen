#pragma once

#include "cacheconfig.h"
#include "hellfire/nlevels.h"

namespace asset {

struct Hellfire {
	hellfire::NLevels nlevels;

	static Hellfire LoadAssets(const CacheConfig& config);
	void UnloadAssets();
};

}  // namespace asset
