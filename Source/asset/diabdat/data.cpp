#include "data.h"

#include <string>
#include <string_view>

#include "../asset.h"
#include "../cacheconfig.h"

namespace asset {
namespace diabdat {

Data Data::LoadDirectory(const CacheConfig& config)
{
	Data dir;

	dir.pentSpn2_cel = Data::LoadFile("PentSpn2.CEL");
	dir.textBox2_cel = Data::LoadFile("TextBox2.CEL");
	dir.textSlid_cel = Data::LoadFile("TextSlid.CEL");
	
	return dir;
}

void Data::UnloadDirectory()
{
	textSlid_cel.UnloadFile();
	textBox2_cel.UnloadFile();
	pentSpn2_cel.UnloadFile();
}

Asset Data::LoadFile(std::string_view szFilename)
{
	static constexpr std::string_view szPathPrefix = "Data\\";
	std::string szPath(szPathPrefix);
	szPath += szFilename;

	return Asset::LoadFile(szPath);
}

}  // namespace diabdat
}  // namespace asset
