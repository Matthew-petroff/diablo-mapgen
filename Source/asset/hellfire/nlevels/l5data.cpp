#include "l5data.h"

#include <string>
#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace hellfire {
namespace nlevels {

L5Data L5Data::LoadDirectory(const CacheConfig& config)
{
	L5Data dir;

	dir.l5_min = L5Data::LoadFile("L5.MIN");
	dir.l5_sol = L5Data::LoadFile("L5.SOL");
	dir.l5_til = L5Data::LoadFile("L5.TIL");

	return dir;
}

void L5Data::UnloadDirectory()
{
	l5_til.UnloadFile();
	l5_sol.UnloadFile();
	l5_min.UnloadFile();
}

Asset L5Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L5Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
