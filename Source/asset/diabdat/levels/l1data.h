#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L1Data {
	// Required assets (used in Spawn)
	Asset l1_min;
	Asset l1_sol;
	Asset l1_til;

	// Retail assets
	Asset rnd6_dun;

	// Retail assets, single-player
	Asset banner1_dun;
	Asset banner2_dun;
	Asset sKngDo_dun;

	// These files are only ever used by setmaps.cpp:LoadSetMap(), which is
	// never called. These members are only defined to remove the usage of
	// LoadFileInMem.
	Asset lv1MazeA_dun;
	Asset lv1MazeB_dun;
	Asset sklKng1_dun;
	Asset sklKng2_dun;
	Asset vile1_dun;
	Asset vile2_dun;

	static L1Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
