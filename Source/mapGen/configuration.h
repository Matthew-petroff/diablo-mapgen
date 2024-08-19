#pragma once

#include <cstdint>
#include <string>
#include <optional>

#include "../analyzer/scannerName.h"

struct Configuration {
	static Configuration ParseArguments(int argc, char **argv);

	uint32_t startSeed = 0;
	uint32_t seedCount = 1;
	std::string seedFile;
	Scanners scanner = Scanners::None;
	bool quiet = false;
	bool asciiLevels = false;
	bool exportLevels = false;
	std::optional<uint32_t> target = std::nullopt;
	std::string targetStr = "Naj's Puzzler";
	bool verbose = false;
};
