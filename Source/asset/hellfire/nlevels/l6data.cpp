#include "l6data.h"

#include <string>
#include <string_view>

#include "../../asset.h"

namespace asset {
namespace hellfire {
namespace nlevels {

L6Data L6Data::LoadDirectory()
{
	L6Data dir;

	dir.l6_min = L6Data::LoadFile("L6.MIN");
	dir.l6_sol = L6Data::LoadFile("L6.SOL");
	dir.l6_til = L6Data::LoadFile("L6.TIL");

	return dir;
}

void L6Data::UnloadDirectory()
{
	l6_til.UnloadFile();
	l6_sol.UnloadFile();
	l6_min.UnloadFile();
}

Asset L6Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L6Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace nlevels
}  // namespace hellfire
}  // namespace asset
