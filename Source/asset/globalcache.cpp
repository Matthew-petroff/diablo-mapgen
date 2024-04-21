#include "globalcache.h"

#include "cacheconfig.h"
#include "diabdat.h"
#include "hellfire.h"

namespace asset {

GlobalCache& GlobalCache::Get()
{
	static GlobalCache cache;

	return cache;
}

void GlobalCache::LoadAssets(const CacheConfig& config)
{
	diabdat = Diabdat::LoadAssets(config);
	if (config.retailMode == RetailMode::Hellfire) {
		hellfire = Hellfire::LoadAssets(config);
	}
}

void GlobalCache::UnloadAssets()
{
	diabdat.UnloadAssets();
	hellfire.UnloadAssets();
}

}  // namespace asset
