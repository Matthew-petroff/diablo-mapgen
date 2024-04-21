#pragma once

#include "cacheconfig.h"
#include "diabdat/data.h"
#include "diabdat/levels.h"
#include "diabdat/plrgfx.h"

namespace asset {

struct Diabdat {
	diabdat::Levels levels;

	// This directory is only ever used by stores.cpp:InitStores(), which is
	// never called. This member is only defined to remove the usage of
	// LoadFileInMem.
	diabdat::Data data;

	// This directory is only ever used by lighting.cpp:MakeLightTable(), which
	// is never called. This member is only defined to remove the usage of
	// LoadFileInMem.
	diabdat::PlrGfx plrGfx;

	static Diabdat LoadAssets(const CacheConfig& config);
	void UnloadAssets();
};

}  // namespace asset
