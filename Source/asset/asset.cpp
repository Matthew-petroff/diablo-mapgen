#include "asset.h"

#include <string>
#include <utility>

#include "../all.h"

namespace asset {

Asset::~Asset()
{
	UnloadFile();
}

Asset Asset::LoadFile(std::string szPath)
{
	Asset asset;

	asset.pData = LoadFileInMem(std::move(szPath), &asset.dwSize);

	return asset;
}

void Asset::UnloadFile()
{
	dwSize = 0;
	MemFreeDbg(pData);
}

}  // namespace asset
