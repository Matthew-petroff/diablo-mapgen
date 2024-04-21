#include "nlevels.h"

#include "../cacheconfig.h"
#include "nlevels/l5data.h"
#include "nlevels/l6data.h"
#include "nlevels/towndata.h"

namespace asset {
namespace hellfire {

NLevels NLevels::LoadDirectory(const CacheConfig& config)
{
	NLevels dir;

	dir.l5data = nlevels::L5Data::LoadDirectory(config);
	dir.l6data = nlevels::L6Data::LoadDirectory(config);

	if (!config.ignoreUnusedAssets) {
		dir.townData = nlevels::TownData::LoadDirectory(config);
	}

	return dir;
}

void NLevels::UnloadDirectory()
{
	townData.UnloadDirectory();
	l6data.UnloadDirectory();
	l5data.UnloadDirectory();
}

}  // namespace hellfire
}  // namespace asset
