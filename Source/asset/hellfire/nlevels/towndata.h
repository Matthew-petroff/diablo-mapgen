#pragma once

#include <string_view>

#include "../../asset.h"

namespace asset {
namespace hellfire {
namespace nlevels {

struct TownData {
	Asset town_min;
	Asset town_sol;
	Asset town_til;

	static TownData LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
