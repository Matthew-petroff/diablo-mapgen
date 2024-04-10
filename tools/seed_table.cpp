#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string_view>

static void showUsage(std::string_view programName) {
	std::cout << "Usage: " << programName << " [level] seed\n";
}

template<typename resultType, typename intermediateType>
std::optional<resultType> parseNumber(std::string_view numericString, intermediateType minValue, intermediateType maxValue) {
	intermediateType value = 0;
	switch (std::from_chars(numericString.data(), numericString.data() + numericString.size(), value).ec) {
	case std::errc():
		if (minValue <= value && value <= maxValue) {
			return static_cast<resultType>(value);
		}
		// else fall through
	case std::errc::result_out_of_range:
		std::cout << numericString << " is outside the expected range of " << minValue << " to " << maxValue << ".\n";
		break;

	case std::errc::invalid_argument:
	default:
		std::cout << numericString << " does not appear to be numeric.\n";
		break;
	}

	return {};
}

static std::optional<uint8_t> parseLevel(std::string_view numericString) {
	return parseNumber<uint8_t, int8_t>(numericString, 0, 16);
}

static std::optional<uint32_t> parseSeed(std::string_view numericString) {
	return parseNumber<uint32_t, int64_t>(numericString, std::numeric_limits<int32_t>::min(), std::numeric_limits<uint32_t>::max());
}

constexpr uint32_t Multiplier = 22695477; // 0x015A4E35;
constexpr uint32_t Increment = 1;
constexpr uint32_t InverseMultiplier = 690295837; // computed using PowerMod[22695477,-1,2^32] in Wolfram Playground

static uint32_t advanceRng(uint32_t state) {
	return state * Multiplier + Increment;
}

static uint32_t backtrackRng(uint32_t state) {
	return (state - Increment) * InverseMultiplier;
}

struct Quests {
	bool butcher = true;
	bool pwater = true;
	bool skelking = true;
	bool ltbanner = true;
	bool blood = true;
	bool schamb = true;
	bool blind = true;
	bool anvil = true;
	bool warlord = true;
};

static int32_t absShiftMod(uint32_t state, int32_t limit) {
	int32_t seed = static_cast<int32_t>(state);
	seed = seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : std::abs(seed);
	return (seed >> 16) % limit;
}

static Quests determineActiveQuests(uint32_t seed) {
	Quests activeQuests{};

	uint32_t state = advanceRng(seed);
	if (absShiftMod(state, 2) == 0) {
		activeQuests.skelking = false;
	}
	else {
		activeQuests.pwater = false;
	}

	state = advanceRng(state);
	switch (absShiftMod(state, 3)) {
	case 0:
		activeQuests.butcher = false;
		break;
	case 1:
		activeQuests.ltbanner = false;
		break;
	case 2:
		// Gharbad the Weak doesn't impact map generation
		//activeQuests.garbund = false;
		break;
	case std::numeric_limits<int32_t>::min():
	default:
		std::cerr << "Q_BUTCHER, Q_LTBANNER, Q_GARBUND all active\n";
		break;
	}

	state = advanceRng(state);
	switch (absShiftMod(state, 3)) {
	case 0:
		activeQuests.blind = false;
		break;
	case 1:
		// The Magic Rock doesn't impact map generation
		//activeQuests.rock = false;
		break;
	case 2:
		activeQuests.blood = false;
		break;
	case std::numeric_limits<int32_t>::min():
	default:
		std::cerr << "Q_BLIND, Q_ROCK, Q_BLOOD all active\n";
		break;
	}

	state = advanceRng(state);
	switch (absShiftMod(state, 3)) {
	case 0:
		// Black Mushroom doesn't impact map generation
		//activeQuests.mushroom = false;
		break;
	case 1:
		// Zhar the Mad doesn't impact map generation
		//activeQuests.zhar = false;
		break;
	case 2:
		activeQuests.anvil = false;
		break;
	case std::numeric_limits<int32_t>::min():
	default:
		std::cerr << "Q_MUSHROOM, Q_ZHAR, Q_ANVIL all active\n";
		break;
	}

	if (absShiftMod(advanceRng(state), 2) == 0) {
		// Lachdanan doesn't impact map generation
		//activeQuests.veil = false;
	}
	else {
		activeQuests.warlord = false;
	}

	return activeQuests;
}

struct GameState {
	uint32_t startingSeed;

	std::array<int32_t, 17> seedTable{};

	GameState() = delete;

	GameState(uint32_t state) : startingSeed(state) {
		for (auto& dungeonSeed : seedTable) {
			state = advanceRng(state);
			dungeonSeed = std::abs(static_cast<int32_t>(state));
		}
	}

	GameState(uint8_t level, uint32_t seed, bool useNegatedState = false) {
		seedTable[level] = seed;

		uint32_t state = useNegatedState ? -static_cast<int32_t>(seed) : seed;
		for (int i = level - 1; i >= 0; --i) {
			state = backtrackRng(state);
			seedTable[i] = std::abs(static_cast<int32_t>(state));
		}
		startingSeed = backtrackRng(state);

		state = useNegatedState ? -static_cast<int32_t>(seed) : seed;
		for (int i = level + 1; i < seedTable.size(); ++i) {
			state = advanceRng(state);
			seedTable[i] = std::abs(static_cast<int32_t>(state));
		}
	}
};

static void renderSeedTable(const GameState &state) {

	time_t startingTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(state.startingSeed)));
	std::cout << "Starting seed: " << state.startingSeed << " (" << std::put_time(std::gmtime(&startingTime), "%Y-%m-%d %H:%M:%S");
	if (315532800 <= state.startingSeed && state.startingSeed <= 2177452799U) {
		std::cout << " can be reached by setting the system time on windows)\n";
	}
	else if (state.startingSeed < 315532800) {
		std::cout << " requires an NT based windows or using the mac version)\n";
	}
	else {
		std::cout << " requires a modern recompile)\n";
	}

	Quests activeQuests = determineActiveQuests(state.seedTable[15]);

	for (auto i = 0; i < state.seedTable.size(); ++i) {
		std::cout << std::setw(2) << i << ":" << std::setw(11) << state.seedTable[i];
		if (i == 2) {
			if (activeQuests.butcher && activeQuests.pwater) {
				std::cout << " (with The Butcher and Poison Water Supply [Q_BUTCHER && Q_PWATER])";
			}
			else if (activeQuests.butcher) {
				std::cout << " (with The Butcher [Q_BUTCHER])";
			}
			else if (activeQuests.pwater) {
				std::cout << " (with Poisoned Water Supply [Q_PWATER])";
			}
		}
		else if (i == 3 && activeQuests.skelking) {
			std::cout << " (with The Curse of King Leoric [Q_SKELKING])";
		}
		else if (i == 4 && activeQuests.ltbanner) {
			std::cout << " (with Ogden's Sign [Q_LTBANNER])";
		}
		else if (i == 5 && activeQuests.blood) {
			std::cout << " (with Valor [Q_BLOOD])";
		}
		else if (i == 6 && activeQuests.schamb) {
			std::cout << " (with Chamber of Bone [Q_SCHAMB])";
		}
		else if (i == 7 && activeQuests.blind) {
			std::cout << " (with Halls of the Blind [Q_BLIND])";
		}
		else if (i == 10 && activeQuests.anvil) {
			std::cout << " (with Anvil of Fury [Q_ANVIL])";
		}
		else if (i == 13 && activeQuests.warlord) {
			std::cout << " (with Warlord of Blood [Q_ANVIL])";
		}
		std::cout << "\n";
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		showUsage(argv[0]);
		return 1;
	}

	std::optional<uint8_t> level;
	if (argc > 2) {
		level = parseLevel(*++argv);
		if (!level) {
			std::cerr << "Could not parse level.\n";
			return 1;
		}
		if (*level == 0) {
			std::cout << "Town seeds are frequently rerolled, if you're analysing a save this won't give you a useful table for comparison.\n\n";
		}
	}

	std::optional<uint32_t> seed = parseSeed(*++argv);
	if (!seed) {
		std::cerr << "Could not parse seed.\n";
		return 1;
	}

	GameState state = level ? GameState(*level, *seed) : GameState(*seed);

	renderSeedTable(state);

	if (level) {
		if (0 < *seed && *seed <= std::numeric_limits<int32_t>::max()) {
			std::cout << "\nSeed could have been naturally generated from a negated state, alternative table follows:\n";
			state = GameState(*level, *seed, true);
			renderSeedTable(state);
		}
		else if (*seed == 0 || *seed == static_cast<uint32_t>(std::numeric_limits<int32_t>::min())) {
			std::cout << "Only one state could lead to that seed.\n";
		}
		else {
			std::cout << "This seed can only be used with a modified save.\n";
		}
	}

	return 0;
}
