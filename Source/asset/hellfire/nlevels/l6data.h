#pragma once

#include <string_view>

#include "../../asset.h"

namespace asset {
namespace hellfire {
namespace nlevels {

struct L6Data {
	Asset l6_min;
	Asset l6_sol;
	Asset l6_til;

	static L6Data LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
