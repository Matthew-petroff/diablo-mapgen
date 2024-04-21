#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L2Data {
	// Required assets
	Asset l2_min;
	Asset l2_sol;
	Asset l2_til;

	// Single-player assets
	Asset blind2_dun;
	Asset blood2_dun;
	Asset bonestr1_dun;

	// The following members are only defined to remove the usage of
	// LoadFileInMem:

	// These files are not relevant to map generation.
	Asset blind1_dun;
	Asset blood1_dun;
	Asset bonestr2_dun;

	// These files are only ever used by setmaps.cpp:LoadSetMap(), which is
	// never called.
	Asset bonecha1_dun;
	Asset bonecha2_dun;

	static L2Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
