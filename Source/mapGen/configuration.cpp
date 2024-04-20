#include "configuration.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

#include "../analyzer/scannerName.h"

namespace {

static void printHelp()
{
	std::cout << "--help         Print this message and exit" << std::endl;
	std::cout << "--ascii        Print ASCII version of levels" << std::endl;
	std::cout << "--export       Export levels as .dun files" << std::endl;
	std::cout << "--scanner <#>  How to analyze levels [default: none]" << std::endl;
	std::cout << "                   none: No analyzing" << std::endl;
	std::cout << "                   puzzler: Search for Naj's Puzzler on level 9" << std::endl;
	std::cout << "                   path: Estimate the time to complete the game" << std::endl;
	std::cout << "                   warp: Find seeds with a warp on level 15" << std::endl;
	std::cout << "                   stairs: Look for stairs with a very short distance to level 9" << std::endl;
	std::cout << "                   pattern: Search a set tile pattern" << std::endl;
	std::cout << "                   gameseed: Search for GameSeeds with LevelSeed" << std::endl;
	std::cout << "--start <#>    The seed to start from" << std::endl;
	std::cout << "--count <#>    The number of seeds to process" << std::endl;
	std::cout << "--seeds <#>    A file to read seeds from" << std::endl;
	std::cout << "--target <#>   The target for the current filter [default: 420]" << std::endl;
	std::cout << "--quiet        Do print status messages" << std::endl;
	std::cout << "--verbose      Print out details about seeds" << std::endl;
}

}  // namespace

Configuration Configuration::ParseArguments(int argc, char **argv)
{
	Configuration config;

	bool fromFile = false;
	bool hasCount = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--help") {
			printHelp();
			exit(0);
		} else if (arg == "--quiet") {
			config.quiet = true;
		} else if (arg == "--ascii") {
			config.asciiLevels = true;
		} else if (arg == "--export") {
			config.exportLevels = true;
		} else if (arg == "--scanner") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --scanner" << std::endl;
				exit(255);
			}
			std::string_view name = argv[i];
			std::optional<Scanners> scanner = Scanners_FromDisplayName(name);
			if (!scanner.has_value()) {
				std::cerr << "Unknown scanner: " << name << std::endl;
				exit(255);
			}
			config.scanner = *scanner;
		} else if (arg == "--seeds") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing filename for --seeds" << std::endl;
				exit(255);
			}
			fromFile = true;
			config.seedFile = argv[i];
		} else if (arg == "--start") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --start" << std::endl;
				exit(255);
			}
			config.startSeed = std::stoll(argv[i]);
		} else if (arg == "--count") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --count" << std::endl;
				exit(255);
			}
			hasCount = true;
			config.seedCount = std::stoll(argv[i]);
		} else if (arg == "--target") {
			i++;
			if (argc <= i) {
				std::cerr << "Missing value for --target" << std::endl;
				exit(255);
			}
			config.target = std::stoll(argv[i]);
		} else if (arg == "--verbose") {
			config.verbose = true;
		} else {
			std::cerr << "Unknown argument: " << arg << std::endl;
			exit(255);
		}
	}

	if (fromFile && !hasCount) {
		config.seedCount = std::numeric_limits<uint32_t>::max();
	}

	return config;
}
