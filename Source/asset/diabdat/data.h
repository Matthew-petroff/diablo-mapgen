#pragma once

#include <string_view>

#include "../asset.h"
#include "../cacheconfig.h"

namespace asset {
namespace diabdat {

struct Data {
	Asset pentSpn2_cel;
	Asset textBox2_cel;
	Asset textSlid_cel;

	static Data LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace diabdat
}  // namespace asset
