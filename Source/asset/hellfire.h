#pragma once

#include "hellfire/nlevels.h"

namespace asset {

struct Hellfire {
	hellfire::NLevels nlevels;

	static Hellfire LoadAssets();
	void UnloadAssets();
};

}  // namespace asset
