#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L3Data {
	// Required assets
	Asset l3_min;
	Asset l3_sol;
	Asset l3_til;

	// Single-player assets
	Asset anvil_dun;

	// This file is only ever used by setmaps.cpp:LoadSetMap(), which is
	// never called. This member is only defined to remove the usage of
	// LoadFileInMem.
	Asset foulwatr_dun;

	static L3Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
