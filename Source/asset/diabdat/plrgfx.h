#pragma once

#include "../asset.h"
#include "../cacheconfig.h"

namespace asset {
namespace diabdat {

struct PlrGfx {
	Asset infra_trn;
	Asset stone_trn;

	static PlrGfx LoadDirectory(const CacheConfig& config);
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace diabdat
}  // namespace asset
