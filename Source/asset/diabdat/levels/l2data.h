#pragma once

#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L2Data {
	Asset blind1_dun;
	Asset blind2_dun;
	Asset blood1_dun;
	Asset blood2_dun;
	Asset bonecha1_dun;
	Asset bonecha2_dun;
	Asset bonestr1_dun;
	Asset bonestr2_dun;
	Asset l2_min;
	Asset l2_sol;
	Asset l2_til;

	static L2Data LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
