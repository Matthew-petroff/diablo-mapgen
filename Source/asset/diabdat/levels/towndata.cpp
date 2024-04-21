#include "towndata.h"

#include <string>
#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

TownData TownData::LoadDirectory()
{
	TownData dir;

	dir.town_min = TownData::LoadFile("town.MIN");
	dir.town_sol = TownData::LoadFile("town.SOL");
	dir.town_til = TownData::LoadFile("town.TIL");

	return dir;
}

void TownData::UnloadDirectory()
{
	town_til.UnloadFile();
	town_sol.UnloadFile();
	town_min.UnloadFile();
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
