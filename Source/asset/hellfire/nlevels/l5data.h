#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace hellfire {
namespace nlevels {

struct L5Data {
	Asset l5_min;
	Asset l5_sol;
	Asset l5_til;

	static L5Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
