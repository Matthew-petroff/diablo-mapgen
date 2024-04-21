#pragma once

#include "../asset.h"

namespace asset {
namespace diabdat {

struct PlrGfx {
	Asset infra_trn;
	Asset stone_trn;

	static PlrGfx LoadDirectory();
	void UnloadDirectory();

private:
	static Asset LoadFile(std::string_view szFilename);
};

}  // namespace diabdat
}  // namespace asset
