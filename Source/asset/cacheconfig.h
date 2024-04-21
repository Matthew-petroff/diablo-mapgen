#pragma once

namespace asset {

enum class RetailMode {
	Undefined = 0,

	// Loads assets used by Diablo Spawn.
	Spawn,
	// Loads assets used by Diablo Retail.
	Retail,
	// Loads assets used by Hellfire.
	Hellfire,
};

enum class GameMode {
	Undefined = 0,

	// Loads assets used in single-player.
	SinglePlayer,
	// Loads assets used in multiplayer.
	Multiplayer,
};

struct CacheConfig {
	RetailMode retailMode;
	GameMode gameMode;
	bool ignoreUnusedAssets;
};

}  // namespace asset
