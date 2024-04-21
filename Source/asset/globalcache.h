#pragma once

#include "diabdat.h"
#include "hellfire.h"

namespace asset {

class GlobalCache {
private:
	constexpr GlobalCache() = default;

public:
	Diabdat diabdat;
#ifdef HELLFIRE
	Hellfire hellfire;
#endif

	static GlobalCache& Get();

	void LoadAssets();
	void UnloadAssets();
};

}  // namespace asset
