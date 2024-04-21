#include "nlevels.h"

#include "nlevels/l5data.h"
#include "nlevels/l6data.h"
#include "nlevels/towndata.h"

namespace asset {
namespace hellfire {

NLevels NLevels::LoadDirectory()
{
	NLevels dir;

	dir.l5data = nlevels::L5Data::LoadDirectory();
	dir.l6data = nlevels::L6Data::LoadDirectory();
	dir.townData = nlevels::TownData::LoadDirectory();

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
