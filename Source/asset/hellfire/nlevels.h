#pragma once

#include "nlevels/l5data.h"
#include "nlevels/l6data.h"
#include "nlevels/towndata.h"

namespace asset {
namespace hellfire {

struct NLevels {
	nlevels::L5Data l5data;
	nlevels::L6Data l6data;
	nlevels::TownData townData;

	static NLevels LoadDirectory();
	void UnloadDirectory();
};

}  // namespace hellfire
}  // namespace asset
