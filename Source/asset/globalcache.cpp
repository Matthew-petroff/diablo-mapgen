#include "globalcache.h"

#include "diabdat.h"
#include "hellfire.h"

namespace asset {

GlobalCache& GlobalCache::Get()
{
	static GlobalCache cache;

	return cache;
}

void GlobalCache::LoadAssets()
{
	diabdat = Diabdat::LoadAssets();
#ifdef HELLFIRE
	hellfire = Hellfire::LoadAssets();
#endif
}

void GlobalCache::UnloadAssets()
{
	diabdat.UnloadAssets();
#ifdef HELLFIRE
	hellfire.UnloadAssets();
#endif
}

}  // namespace asset
