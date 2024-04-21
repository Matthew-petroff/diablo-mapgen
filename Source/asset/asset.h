#pragma once

#include <string>
#include <utility>

#include "../all.h"

namespace asset {

class Asset {
public:
	constexpr Asset() : pData(nullptr), dwSize(0) {}
	~Asset();

	// Assets should never be copied.
	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;

	// Assets should be movable.
	constexpr Asset(Asset&& other) :
		pData(std::move(other.pData)), dwSize(std::move(other.dwSize))
	{
		other.pData = nullptr;
		other.dwSize = 0;
	}

	constexpr Asset& operator=(Asset&& other)
	{
		pData = std::move(other.pData);
		dwSize = std::move(other.dwSize);
		other.pData = nullptr;
		other.dwSize = 0;

		return *this;
	}

	static Asset LoadFile(std::string pszPath);
	void UnloadFile();

	constexpr const BYTE* GetData() const
	{
		return pData;
	}

	constexpr DWORD GetSize() const
	{
		return dwSize;
	}

private:
	BYTE* pData;
	DWORD dwSize;
};

}  // namespace asset
