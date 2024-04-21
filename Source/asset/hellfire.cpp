#include "hellfire.h"

#include "cacheconfig.h"
#include "hellfire/nlevels.h"

namespace asset {

Hellfire Hellfire::LoadAssets(const CacheConfig& config)
{
	Hellfire archive;

	archive.nlevels = hellfire::NLevels::LoadDirectory(config);

	return archive;
}

void Hellfire::UnloadAssets()
{
	nlevels.UnloadDirectory();
}

}  // namespace asset
