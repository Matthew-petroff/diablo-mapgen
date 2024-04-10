#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string_view>
#include <vector>
#include <map>

static void showUsage(std::string_view programName)
{
	std::cerr << "Usage: " << programName << " target_timestamp level seeds.txt\n";
}

template <typename resultType, typename intermediateType>
std::optional<resultType> parseNumber(std::string_view numericString, intermediateType minValue, intermediateType maxValue)
{
	intermediateType value = 0;
	switch (std::from_chars(numericString.data(), numericString.data() + numericString.size(), value).ec) {
	case std::errc():
		if (minValue <= value && value <= maxValue) {
			return static_cast<resultType>(value);
		}
		// else fall through
	case std::errc::result_out_of_range:
		std::cerr << numericString << " is outside the expected range of " << minValue << " to " << maxValue << ".\n";
		break;

	case std::errc::invalid_argument:
	default:
		std::cerr << numericString << " does not appear to be numeric.\n";
		break;
	}

	return {};
}

static std::optional<uint8_t> parseLevel(std::string_view numericString)
{
	return parseNumber<uint8_t, int8_t>(numericString, 0, 16);
}

static std::optional<uint32_t> parseSeed(std::string_view numericString)
{
	return parseNumber<uint32_t, int64_t>(numericString, std::numeric_limits<int32_t>::min(), std::numeric_limits<uint32_t>::max());
}

constexpr uint32_t Multiplier        = 22695477; // 0x015A4E35;
constexpr uint32_t Increment         = 1;
constexpr uint32_t InverseMultiplier = 690295837; // computed using PowerMod[22695477,-1,2^32] in Wolfram Playground

static uint32_t advanceRng(uint32_t state)
{
	return state * Multiplier + Increment;
}

static uint32_t backtrackRng(uint32_t state)
{
	return (state - Increment) * InverseMultiplier;
}

static int32_t absShiftMod(uint32_t state, int32_t limit)
{
	int32_t seed = static_cast<int32_t>(state);
	seed         = seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : std::abs(seed);
	return (seed >> 16) % limit;
}

int main(int argc, char *argv[])
{
	if (argc < 4) {
		showUsage(argv[0]);
		return 1;
	}

	std::optional<uint32_t> targetTimestamp = parseSeed(argv[1]);
	if (!targetTimestamp) {
		std::cerr << "Could not parse target timestamp.\n";
		return 1;
	}

	std::optional<uint8_t> level = parseLevel(argv[2]);
	if (!level) {
		std::cerr << "Could not parse level.\n";
		return 1;
	}
	if (*level == 0) {
		std::cerr << "Town seeds are frequently rerolled, if you're analysing a save this won't give you a useful table for comparison.\n\n";
	}

	std::fstream seedsFile;
	seedsFile.open(argv[3]);
	if (!seedsFile.is_open()) {
		std::cerr << "Unable to open " << argv[3] << "\n";
		return 1;
	}

	std::map<uint32_t, std::vector<uint32_t>> seedsByDelta {};

	for (std::string line; std::getline(seedsFile, line);) {
		std::optional<uint32_t> seed = parseSeed(line);
		if (!seed) {
			std::cerr << "Could not parse seed.\n";
			return 1;
		}

		uint32_t state = *seed;
		for (int i = *level - 1; i >= 0; --i) {
			state        = backtrackRng(state);
		}
		uint32_t startingSeed = backtrackRng(state);

		uint32_t delta = static_cast<uint32_t>(abs(static_cast<int64_t>(*targetTimestamp) - startingSeed));

		if (*seed <= std::numeric_limits<int32_t>::max()) {
			state = -static_cast<int32_t>(*seed);
			for (int i = *level - 1; i >= 0; --i) {
				state = backtrackRng(state);
			}
			startingSeed = backtrackRng(state);

			delta = std::min(delta, static_cast<uint32_t>(abs(static_cast<int64_t>(*targetTimestamp) - startingSeed)));
		}

		if (seedsByDelta.contains(delta)) {
			seedsByDelta[delta].push_back(*seed);
		}
		else {
			seedsByDelta.emplace(delta, std::vector { *seed });
		}

	}

	for (auto &[_, seeds] : seedsByDelta) {
		for (auto seed : seeds) {
			std::cout << seed << "\n";
		}
	}

	return 0;
}
