#include "diabdat.h"

#include "diabdat/data.h"
#include "diabdat/levels.h"
#include "diabdat/plrgfx.h"

namespace asset {

Diabdat Diabdat::LoadAssets()
{
	Diabdat archive;

	archive.data = diabdat::Data::LoadDirectory();
	archive.levels = diabdat::Levels::LoadDirectory();
	archive.plrGfx = diabdat::PlrGfx::LoadDirectory();

	return archive;
}

void Diabdat::UnloadAssets()
{
	plrGfx.UnloadDirectory();
	levels.UnloadDirectory();
	data.UnloadDirectory();
}

}  // namespace asset
