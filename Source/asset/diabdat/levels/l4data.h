#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L4Data {
	// Required assets
	Asset diab1_dun;
	Asset diab2a_dun;
	Asset diab2b_dun;
	Asset diab3a_dun;
	Asset diab3b_dun;
	Asset diab4a_dun;
	Asset diab4b_dun;
	Asset l4_min;
	Asset l4_sol;
	Asset l4_til;

	// Single-player assets
	Asset warlord_dun;

	// Asset irrelevant for map generation
	Asset warlord2_dun;

	// Multiplayer
	Asset vile1_dun;

	static L4Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
