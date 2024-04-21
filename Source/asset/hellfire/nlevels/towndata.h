#pragma once

#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace hellfire {
namespace nlevels {

struct TownData {
	Asset town_sol;

	static TownData LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
