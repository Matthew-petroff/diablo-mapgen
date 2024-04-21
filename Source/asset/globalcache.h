#pragma once

#include "cacheconfig.h"
#include "diabdat.h"
#include "hellfire.h"

namespace asset {

class GlobalCache {
private:
	constexpr GlobalCache() = default;

public:
	Diabdat diabdat;
	Hellfire hellfire;

	static GlobalCache& Get();

	void LoadAssets(const CacheConfig& config);
	void UnloadAssets();
};

}  // namespace asset
