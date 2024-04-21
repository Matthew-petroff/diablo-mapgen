#pragma once

#include "diabdat/data.h"
#include "diabdat/levels.h"
#include "diabdat/plrgfx.h"

namespace asset {

struct Diabdat {
	diabdat::Data data;
	diabdat::Levels levels;
	diabdat::PlrGfx plrGfx;

	static Diabdat LoadAssets();
	void UnloadAssets();
};

}  // namespace asset
