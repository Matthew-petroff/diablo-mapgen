#include "scannerName.h"

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

std::optional<Scanners> Scanners_FromDisplayName(std::string_view name)
{
	// For any new scanners, just add to the pre-sorted lookup table.
	constexpr std::pair<std::string_view, Scanners> displayNames[] = {
		{ "gameseed", Scanners::GameSeed },
		{ "none", Scanners::None },
		{ "path", Scanners::Path },
		{ "pattern", Scanners::Pattern },
		{ "puzzler", Scanners::Puzzler },
		{ "quest", Scanners::Quest },
		{ "stairs", Scanners::Stairs },
		{ "warp", Scanners::Warp },
	};
	static_assert(
		std::is_sorted(
			std::cbegin(displayNames), std::cend(displayNames)));

	std::pair key = std::make_pair(name, Scanners::None);
	auto result =
		std::equal_range(
			std::cbegin(displayNames),
			std::cend(displayNames),
			key,
			[](auto& lhs, auto& rhs) {
				return lhs.first < rhs.first;
			});
	if (result.first == std::cend(displayNames)) {
		return std::nullopt;
	}

	return result.first->second;
}

std::optional<std::string> Scanners_ToDisplayName(Scanners scanner)
{
	switch (scanner) {
		case Scanners::None: {
			static constexpr std::string_view displayName = "none";
			return std::string(displayName);
		}
		case Scanners::Path: {
			static constexpr std::string_view displayName = "path";
			return std::string(displayName);
		}
		case Scanners::Quest: {
			static constexpr std::string_view displayName = "quest";
			return std::string(displayName);
		}
		case Scanners::Puzzler: {
			static constexpr std::string_view displayName = "puzzler";
			return std::string(displayName);
		}
		case Scanners::Stairs: {
			static constexpr std::string_view displayName = "stairs";
			return std::string(displayName);
		}
		case Scanners::Warp: {
			static constexpr std::string_view displayName = "warp";
			return std::string(displayName);
		}
		case Scanners::Pattern: {
			static constexpr std::string_view displayName = "pattern";
			return std::string(displayName);
		}
		case Scanners::GameSeed: {
			static constexpr std::string_view displayName = "gameseed";
			return std::string(displayName);
		}
		default: {
			return std::nullopt;
		}
	}
}
