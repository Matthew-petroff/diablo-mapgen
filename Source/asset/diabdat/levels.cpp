#include "levels.h"

#include "../cacheconfig.h"
#include "levels/l1data.h"
#include "levels/l2data.h"
#include "levels/l3data.h"
#include "levels/l4data.h"
#include "levels/towndata.h"

namespace asset {
namespace diabdat {

Levels Levels::LoadDirectory(const CacheConfig& config)
{
	Levels dir;

	dir.l1data = levels::L1Data::LoadDirectory(config);
	
	if (config.retailMode != RetailMode::Spawn) {
		dir.l2data = levels::L2Data::LoadDirectory(config);
		dir.l3data = levels::L3Data::LoadDirectory(config);
		dir.l4data = levels::L4Data::LoadDirectory(config);
	}

	if (!config.ignoreUnusedAssets) {
		dir.townData = levels::TownData::LoadDirectory(config);
	}

	return dir;
}

void Levels::UnloadDirectory()
{
	townData.UnloadDirectory();

	l4data.UnloadDirectory();
	l3data.UnloadDirectory();
	l2data.UnloadDirectory();

	l1data.UnloadDirectory();
}

}  // namespace diabdat
}  // namespace asset
