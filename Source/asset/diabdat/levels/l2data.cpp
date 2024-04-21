#include "l2data.h"

#include <string>
#include <string_view>

#include "../../asset.h"
#include "../../cacheconfig.h"

namespace asset {
namespace diabdat {
namespace levels {

L2Data L2Data::LoadDirectory(const CacheConfig& config)
{
	L2Data dir;

	dir.l2_min = L2Data::LoadFile("L2.MIN");
	dir.l2_sol = L2Data::LoadFile("L2.SOL");
	dir.l2_til = L2Data::LoadFile("L2.TIL");

	if (config.gameMode == GameMode::SinglePlayer) {
		dir.blind2_dun = L2Data::LoadFile("Blind2.DUN");
		dir.blood2_dun = L2Data::LoadFile("Blood2.DUN");
		dir.bonestr1_dun = L2Data::LoadFile("Bonestr1.DUN");
		if (!config.ignoreUnusedAssets) {
			dir.blind1_dun = L2Data::LoadFile("Blind1.DUN");
			dir.blood1_dun = L2Data::LoadFile("Blood1.DUN");
			dir.bonestr2_dun = L2Data::LoadFile("Bonestr2.DUN");

			dir.bonecha1_dun = L2Data::LoadFile("Bonecha1.DUN");
			dir.bonecha2_dun = L2Data::LoadFile("Bonecha2.DUN");
		}
	}

	return dir;
}

void L2Data::UnloadDirectory()
{
	bonecha2_dun.UnloadFile();
	bonecha1_dun.UnloadFile();

	bonestr2_dun.UnloadFile();
	blood1_dun.UnloadFile();
	blind1_dun.UnloadFile();

	bonestr1_dun.UnloadFile();
	blood2_dun.UnloadFile();
	blind2_dun.UnloadFile();

	l2_til.UnloadFile();
	l2_sol.UnloadFile();
	l2_min.UnloadFile();
}

Asset L2Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Levels\\L2Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace levels
}  // namespace diabdat
}  // namespace asset
