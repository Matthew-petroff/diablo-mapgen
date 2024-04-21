#pragma once

#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

struct L3Data {
	Asset anvil_dun;
	Asset foulwatr_dun;
	Asset l3_min;
	Asset l3_sol;
	Asset l3_til;

	static L3Data LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
