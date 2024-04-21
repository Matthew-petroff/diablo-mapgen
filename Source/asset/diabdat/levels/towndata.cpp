#include "towndata.h"

#include <string>
#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

TownData TownData::LoadDirectory(const CacheConfig& config)
{
	TownData dir;

	dir.town_sol = TownData::LoadFile("town.SOL");

	return dir;
}

void TownData::UnloadDirectory()
{
	town_sol.UnloadFile();
}

Asset TownData::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\TownData\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
