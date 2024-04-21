#include "plrgfx.h"

#include <string>
#include <string_view>

#include "../asset.h"
#include "../cacheconfig.h"

namespace asset {
namespace diabdat {

PlrGfx PlrGfx::LoadDirectory(const CacheConfig& config)
{
	PlrGfx dir;

	dir.infra_trn = PlrGfx::LoadFile("Infra.TRN");
	dir.stone_trn = PlrGfx::LoadFile("Stone.TRN");

	return dir;
}

void PlrGfx::UnloadDirectory()
{
	stone_trn.UnloadFile();
	infra_trn.UnloadFile();
}

Asset PlrGfx::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "PlrGFX\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace diabdat
}  // namespace asset
