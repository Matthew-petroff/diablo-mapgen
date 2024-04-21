#include "hellfire.h"

#include "hellfire/nlevels.h"

namespace asset {

Hellfire Hellfire::LoadAssets()
{
	Hellfire archive;

	archive.nlevels = hellfire::NLevels::LoadDirectory();

	return archive;
}

void Hellfire::UnloadAssets()
{
	nlevels.UnloadDirectory();
}

}  // namespace asset
