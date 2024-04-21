#include "l4data.h"

#include <string>
#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

L4Data L4Data::LoadDirectory()
{
	L4Data dir;

	dir.diab1_dun = L4Data::LoadFile("diab1.DUN");
	dir.diab2a_dun = L4Data::LoadFile("diab2a.DUN");
	dir.diab2b_dun = L4Data::LoadFile("diab2b.DUN");
	dir.diab3a_dun = L4Data::LoadFile("diab3a.DUN");
	dir.diab3b_dun = L4Data::LoadFile("diab3b.DUN");
	dir.diab4a_dun = L4Data::LoadFile("diab4a.DUN");
	dir.diab4b_dun = L4Data::LoadFile("diab4b.DUN");
	dir.l4_min = L4Data::LoadFile("L4.MIN");
	dir.l4_sol = L4Data::LoadFile("L4.SOL");
	dir.l4_til = L4Data::LoadFile("L4.TIL");
	dir.vile1_dun = L4Data::LoadFile("Vile1.DUN");
	dir.warlord_dun = L4Data::LoadFile("Warlord.DUN");
	dir.warlord2_dun = L4Data::LoadFile("Warlord2.DUN");

	return dir;
}

void L4Data::UnloadDirectory()
{
	warlord2_dun.UnloadFile();
	warlord_dun.UnloadFile();
	vile1_dun.UnloadFile();
	l4_til.UnloadFile();
	l4_sol.UnloadFile();
	l4_min.UnloadFile();
	diab4b_dun.UnloadFile();
	diab4a_dun.UnloadFile();
	diab3b_dun.UnloadFile();
	diab3a_dun.UnloadFile();
	diab2b_dun.UnloadFile();
	diab2a_dun.UnloadFile();
	diab1_dun.UnloadFile();
}

Asset L4Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L4Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
