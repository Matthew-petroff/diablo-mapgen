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
	std::cerr << "Usage: " << programName << " <target_timestamp> <level> seeds.txt [--verbose]\n";
}

template <typename resultType, typename intermediateType>
std::optional<resultType> parseNumber(std::string_view numericString, intermediateType minValue, intermediateType maxValue)
{
	intermediateType value = 0;
	switch (std::from_chars(numericString.data(), numericString.data() + numericString.size(), value).ec) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif
	case std::errc():
#ifdef __clang__
#pragma clang diagnostic pop
#endif
		if (minValue <= value && value <= maxValue) {
			return static_cast<resultType>(value);
		}
		// else fall through
		[[fallthrough]];
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

static uint32_t absDelta(uint32_t a, uint32_t b)
{
	return static_cast<uint32_t>(abs(static_cast<int64_t>(a) - b));
}

static auto formatDate(uint32_t timestamp)
{
	time_t time = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(timestamp)));
	return std::put_time(std::gmtime(&time), "%Y-%m-%d %H:%M:%S");
}

struct SeedMap {
	uint32_t dungeonSeed;
	uint32_t gameSeed;
	std::optional<uint32_t> altGameSeed {};

	SeedMap() = delete;

	SeedMap(uint32_t dungeonSeed, uint32_t gameSeed)
	    : dungeonSeed(dungeonSeed)
	    , gameSeed(gameSeed)
	{
	}

	SeedMap(uint32_t dungeonSeed, uint32_t gameSeed, uint32_t altGameSeed)
	    : dungeonSeed(dungeonSeed)
	    , gameSeed(gameSeed)
	    , altGameSeed(altGameSeed)
	{
	}
};

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

	std::map<uint32_t, std::vector<SeedMap>> seedsByDelta {};

	for (std::string line; std::getline(seedsFile, line);) {
		std::optional<uint32_t> seed = parseSeed(line);
		if (!seed) {
			std::cerr << "Could not parse seed.\n";
			return 1;
		}

		uint32_t state = *seed;
		for (int i = *level - 1; i >= 0; --i) {
			state = backtrackRng(state);
		}
		uint32_t startingSeed = backtrackRng(state);

		uint32_t delta = absDelta(*targetTimestamp, startingSeed);

		if (*seed <= std::numeric_limits<int32_t>::max()) {
			state = -static_cast<int32_t>(*seed);
			for (int i = *level - 1; i >= 0; --i) {
				state = backtrackRng(state);
			}
			uint32_t negStartingSeed = backtrackRng(state);

			uint32_t negDelta = absDelta(*targetTimestamp, startingSeed);

			if (negDelta < delta) {
				seedsByDelta[negDelta].emplace_back(*seed, negStartingSeed, startingSeed);
			} else {
				seedsByDelta[delta].emplace_back(*seed, startingSeed, negStartingSeed);
			}
		} else {
			seedsByDelta[delta].emplace_back(*seed, startingSeed);
		}
	}

	using namespace std::literals;
	bool verbose = (argc >= 4 && "--verbose"sv == argv[4]);

	for (auto &[_, seeds] : seedsByDelta) {
		for (auto &seed : seeds) {
			std::cout << seed.dungeonSeed;
			if (verbose) {
				std::cout << "\t" << seed.gameSeed << "\t" << formatDate(seed.gameSeed);
				if (seed.altGameSeed) {
					std::cout << "\t" << *seed.altGameSeed << "\t" << formatDate(*seed.altGameSeed);
				}
			}
			std::cout << "\n";
		}
	}

	return 0;
}
