#pragma once

#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L1Data {
	Asset banner1_dun;
	Asset banner2_dun;
	Asset l1_min;
	Asset l1_sol;
	Asset l1_til;
	Asset lv1MazeA_dun;
	Asset lv1MazeB_dun;
	Asset rnd6_dun;
	Asset sklKng1_dun;
	Asset sklKng2_dun;
	Asset sKngDo_dun;
	Asset vile1_dun;
	Asset vile2_dun;

	static L1Data LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
