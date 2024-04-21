#include "l1data.h"

#include <string>
#include <string_view>

#include "../../asset.h"

namespace asset {
namespace diabdat {
namespace levels {

L1Data L1Data::LoadDirectory()
{
	L1Data dir;

	dir.banner1_dun = L1Data::LoadFile("Banner1.DUN");
	dir.banner2_dun = L1Data::LoadFile("Banner2.DUN");
	dir.l1_min = L1Data::LoadFile("L1.MIN");
	dir.l1_sol = L1Data::LoadFile("L1.SOL");
	dir.l1_til = L1Data::LoadFile("L1.TIL");
	dir.lv1MazeA_dun = L1Data::LoadFile("Lv1MazeA.DUN");
	dir.lv1MazeB_dun = L1Data::LoadFile("Lv1MazeB.DUN");
	dir.rnd6_dun = L1Data::LoadFile("rnd6.DUN");
	dir.sklKng1_dun = L1Data::LoadFile("SklKng1.DUN");
	dir.sklKng2_dun = L1Data::LoadFile("SklKng2.DUN");
	dir.sKngDo_dun = L1Data::LoadFile("SKngDO.DUN");
	dir.vile1_dun = L1Data::LoadFile("Vile1.DUN");
	dir.vile2_dun = L1Data::LoadFile("Vile2.DUN");

	return dir;
}

void L1Data::UnloadDirectory()
{
	vile2_dun.UnloadFile();
	vile1_dun.UnloadFile();
	sKngDo_dun.UnloadFile();
	sklKng2_dun.UnloadFile();
	sklKng1_dun.UnloadFile();
	rnd6_dun.UnloadFile();
	lv1MazeB_dun.UnloadFile();
	lv1MazeA_dun.UnloadFile();
	l1_til.UnloadFile();
	l1_sol.UnloadFile();
	l1_min.UnloadFile();
	banner2_dun.UnloadFile();
	banner1_dun.UnloadFile();
}

Asset L1Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L1Data\\";

	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
