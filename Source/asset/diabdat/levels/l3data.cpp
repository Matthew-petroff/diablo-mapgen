#include "l3data.h"

#include <string>
#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

L3Data L3Data::LoadDirectory()
{
	L3Data dir;

	dir.anvil_dun = L3Data::LoadFile("Anvil.DUN");
	dir.foulwatr_dun = L3Data::LoadFile("Foulwatr.DUN");
	dir.l3_min = L3Data::LoadFile("L3.MIN");
	dir.l3_sol = L3Data::LoadFile("L3.SOL");
	dir.l3_til = L3Data::LoadFile("L3.TIL");

	return dir;
}

void L3Data::UnloadDirectory()
{
	l3_til.UnloadFile();
	l3_sol.UnloadFile();
	l3_min.UnloadFile();
	foulwatr_dun.UnloadFile();
	anvil_dun.UnloadFile();
}

Asset L3Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L3Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
