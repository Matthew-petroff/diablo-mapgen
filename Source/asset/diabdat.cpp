#include "diabdat.h"

#include "cacheconfig.h"
#include "diabdat/data.h"
#include "diabdat/levels.h"
#include "diabdat/plrgfx.h"

namespace asset {

Diabdat Diabdat::LoadAssets(const CacheConfig& config)
{
	Diabdat archive;

	archive.levels = diabdat::Levels::LoadDirectory(config);

	if (!config.ignoreUnusedAssets) {
		archive.data = diabdat::Data::LoadDirectory(config);
		archive.plrGfx = diabdat::PlrGfx::LoadDirectory(config);
	}

	return archive;
}

void Diabdat::UnloadAssets()
{
	plrGfx.UnloadDirectory();
	levels.UnloadDirectory();
	data.UnloadDirectory();
}

}  // namespace asset
