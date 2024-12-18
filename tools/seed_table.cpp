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

static void showUsage(std::string_view programName)
{
	std::cout << "Usage: " << programName << " <level> <dungeon seed>\n";
	std::cout << "       " << programName << " <game seed>\n";
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
		std::cout << numericString << " is outside the expected range of " << minValue << " to " << maxValue << ".\n";
		break;

	case std::errc::invalid_argument:
	default:
		std::cout << numericString << " does not appear to be numeric.\n";
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

struct Quests {
	bool butcher  = true;
	bool pwater   = true;
	bool skelking = true;
	bool garbund  = true;
	bool ltbanner = true;
	bool blood    = true;
	bool rock     = true;
	bool blind    = true;
	bool zhar     = true;
	bool mushroom = true;
	bool anvil    = true;
	bool warlord  = true;
	bool veil     = true;
};

static int32_t absShiftMod(uint32_t state, int32_t limit)
{
	int32_t seed = static_cast<int32_t>(state);
	seed         = seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : std::abs(seed);
	return (seed >> 16) % limit;
}

static Quests determineActiveQuests(uint32_t seed)
{
	Quests activeQuests {};

	uint32_t state = advanceRng(seed);
	if (absShiftMod(state, 2) == 0) {
		activeQuests.skelking = false;
	} else {
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
		activeQuests.garbund = false;
		break;
	case -2:
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
		activeQuests.rock = false;
		break;
	case 2:
		activeQuests.blood = false;
		break;
	case -2:
	default:
		std::cerr << "Q_BLIND, Q_ROCK, Q_BLOOD all active\n";
		break;
	}

	state = advanceRng(state);
	switch (absShiftMod(state, 3)) {
	case 0:
		activeQuests.mushroom = false;
		break;
	case 1:
		activeQuests.zhar = false;
		break;
	case 2:
		activeQuests.anvil = false;
		break;
	case -2:
	default:
		std::cerr << "Q_MUSHROOM, Q_ZHAR, Q_ANVIL all active\n";
		break;
	}

	if (absShiftMod(advanceRng(state), 2) == 0) {
		activeQuests.veil = false;
	} else {
		activeQuests.warlord = false;
	}

	return activeQuests;
}

struct GameState {
	uint32_t startingSeed;

	std::array<int32_t, 17> seedTable {};
	std::array<int32_t, 17> stateTable {};

	GameState() = delete;

	GameState(uint32_t state)
	    : startingSeed(state)
	{
		for (unsigned i = 0; i < seedTable.size(); ++i) {
			state = advanceRng(state);
			setLevelState(i, state);
		}
	}

	GameState(uint8_t level, uint32_t seed)
	{
		setLevelState(level, seed);

		uint32_t state = seed;
		for (int i = level - 1; i >= 0; --i) {
			state = backtrackRng(state);
			setLevelState(i, state);
		}
		startingSeed = backtrackRng(state);

		state = seed;
		for (int i = level + 1; i < seedTable.size(); ++i) {
			state = advanceRng(state);
			setLevelState(i, state);
		}
	}

	void setLevelState(size_t level, uint32_t state)
	{
		stateTable[level] = static_cast<int32_t>(state);
		seedTable[level]  = std::abs(stateTable[level]);
	}
};

static void renderSeedTable(const GameState &state)
{
	time_t startingTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(state.startingSeed)));
	std::cout << "Game seed: " << state.startingSeed << " (" << std::put_time(std::gmtime(&startingTime), "%Y-%m-%d %H:%M:%S") << " ";
	if (state.startingSeed == -1) {
		std::cout << "is the default game seed before 1970/after 2038";
	} else if (315532800 <= state.startingSeed && state.startingSeed <= 2177452799U) {
		std::cout << "can be reached by setting the system time";
	} else if (state.startingSeed < 315532800) {
		std::cout << "requires an NT based Windows and an old BIOS or using the Mac version";
	} else {
		std::cout << "requires a modern recompile";
	}
	std::cout << ")\n";

	Quests activeQuests = determineActiveQuests(state.seedTable[15]);

	for (auto i = 0; i < state.seedTable.size(); ++i) {
		std::cout << std::setw(2) << i << ":" << std::setw(11) << state.seedTable[i] << " [" << std::setw(11) << state.stateTable[i] << "]";
		if (i == 2) {
			if (activeQuests.butcher && activeQuests.pwater) {
				std::cout << " (with The Butcher and Poison Water Supply [Q_BUTCHER && Q_PWATER])";
			} else if (activeQuests.butcher) {
				std::cout << " (with The Butcher [Q_BUTCHER])";
			} else if (activeQuests.pwater) {
				std::cout << " (with Poisoned Water Supply [Q_PWATER])";
			}
		} else if (i == 3 && activeQuests.skelking) {
			std::cout << " (with The Curse of King Leoric [Q_SKELKING])";
		} else if (i == 4) {
			if (activeQuests.garbund && activeQuests.ltbanner) {
				std::cout << " (with Gharbad the Weak and Ogden's Sign [Q_GARBUND && Q_LTBANNER])";
			} else if (activeQuests.garbund) {
				std::cout << " (with Gharbad the Weak [Q_GARBUND])";
			} else if (activeQuests.ltbanner) {
				std::cout << " (with Ogden's Sign [Q_LTBANNER])";
			}
		} else if (i == 5) {
			if (activeQuests.blood && activeQuests.rock) {
				std::cout << " (with Arkaine's Valor and The Magic Rock [Q_BLOOD && Q_ROCK])";
			} else if (activeQuests.blood) {
				std::cout << " (with Arkaine's Valor [Q_BLOOD])";
			} else if (activeQuests.rock) {
				std::cout << " (with The Magic Rock [Q_ROCK])";
			}
		} else if (i == 7 && activeQuests.blind) {
			std::cout << " (with Halls of the Blind [Q_BLIND])";
		} else if (i == 8 && activeQuests.zhar) {
			std::cout << " (with Zhar the Mad [Q_ZHAR])";
		} else if (i == 9 && activeQuests.mushroom) {
			std::cout << " (with Black Mushroom [Q_MUSHROOM])";
		} else if (i == 10 && activeQuests.anvil) {
			std::cout << " (with Anvil of Fury [Q_ANVIL])";
		} else if (i == 13 && activeQuests.warlord) {
			std::cout << " (with Warlord of Blood [Q_WARLORD])";
		} else if (i == 14 && activeQuests.veil) {
			std::cout << " (with Lachdanan [Q_VEIL])";
		}
		std::cout << "\n";
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		showUsage(argv[0]);
		return 1;
	}

	std::optional<uint8_t> level;
	if (argc > 2) {
		level = parseLevel(*++argv);
		if (!level) {
			std::cerr << "Could not parse dungeon level.\n";
			return 1;
		}
		if (*level == 0) {
			std::cout << "Town seeds are frequently rerolled, if you're analysing a save this won't give you a useful table for comparison.\n\n";
		}
	}

	std::optional<uint32_t> seed = parseSeed(*++argv);
	if (!seed) {
		std::cerr << "Could not parse " << (level ? "dungeon" : "game") << " seed.\n";
		return 1;
	}

	GameState state = level ? GameState(*level, *seed) : GameState(*seed);

	renderSeedTable(state);

	if (level) {
		if (0 < *seed && *seed <= std::numeric_limits<int32_t>::max()) {
			std::cout << "\nDungeon seed could have been naturally generated from a negated state, alternative table follows:\n";
			state = GameState(*level, -static_cast<int32_t>(*seed));
			renderSeedTable(state);
		} else if (*seed == 0 || *seed == static_cast<uint32_t>(std::numeric_limits<int32_t>::min())) {
			std::cout << "Only one state could lead to that dungeon seed.\n";
		} else {
			std::cout << "This dungeon seed can only be used with a modified save.\n";
		}
	}

	return 0;
}
