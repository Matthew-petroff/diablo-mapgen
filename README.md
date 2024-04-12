# Diablo MapGen

Diablo MapGen is a tool designed to analyze maps generated in the game Diablo I to identify optimal seed candidates for speedrunning. By analyzing various characteristics of the game maps, such as layout, item distribution, and quest placement, Diablo MapGen assists speedrunners in identifying promising seeds to achieve faster completion times.

<p align="center">
<img width="512" height="512" src="https://github.com/Matthew-petroff/diablo-mapgen/assets/204594/b7f72623-87f1-43f9-9369-c2c7852b186c">
</p>

## Features

- Analyzes Diablo I maps to identify optimal seed candidates.
- Considers factors such as map layout, item distribution, and quest placement.
- Designed to assist speedrunners in achieving faster completion times.

## Building the Project

Diablo MapGen is a CMake-based project, making it easy to build on various platforms. Follow these general steps to build the project on your specific system.

### CLI

As long as you have CMake and a C++ compiler installed you should be able to build using a process similar to

```
mkdir build
cd build
cmake ..
make
```

### GUI

If you use a IDE that support CMake it should be as simple as opening the project or the CMakeList.txt and then pressing the build buttonn in the IDE.

## Usage Examples

As an example, you can run the following command to scan for seeds where Naj's Puzzler can be found on level 9, within a range of seeds that can be produced on all versions of Windows:

```bash
./diablo-mapgen --scanner puzzler --start 315532800 --count 1864050846
```

### Options

- `--ascii`: Print ASCII version of levels.
- `--export`: Export levels as .dun files.
- `--scanner <type>`: How to analyze levels. Available options:
  - `none`: No analysis (default).
  - `warp`: Find seeds with a warp on level 15.
  - `puzzler`: Search for Naj's Puzzler on the level specified by `--target` (default 9).
  - `path`: Estimate the time to complete the levels, skip seeds that take longer then number of secound specified by `--target` (default 420).
  - `stairs`: Look for stairs with a very short distance between them.
  - `pattern`: Search for levels specified by `--target` (default blank) based on tile patterns and print out there level seed.
  - `gameseed`: Search for GameSeeds that generates the LevelSeed given by `--target` (default 9:3916317768).
- `--start <offset>`: The seed to start from.
- `--count <number_of_seeds>`: The number of seeds to process.
- `--seeds <file>`: A file to read seeds from rather then using a sequental range.
- `--target <value>`: A target value to set for the scanner.
- `--quiet`: Do not print progress messages.
- `--verbose`: Print out details about seeds.

### Seed Filtering Strategy

To efficiently analyze seeds start by using the `warp`, or `puzzler` options to quickly filter out seeds based on their criteria. Save the filtered results using the `>` operator to a file for use with the next analyzer. For example:

```
./diablo-mapgen --scanner warp --start 315532800 --count 1864050846 > filtered_seeds.txt
```

Feed the saved seeds to the next analyzer using the --seeds option to further refine the selection. For example:

```
./diablo-mapgen --scanner path --seeds filtered_seeds.txt > narrowed_seeds.txt
```

## Parallel Execution

For parallel execution to analyze maps more efficiently, use the `parallel_mapgen.sh` script provided in this repository. This script allows you to run multiple instances of the Diablo MapGen tool concurrently, leveraging the available CPU threads on your system.

To run the parallel scanner with 4 threads and analyze Diablo I maps using the pattern scanner, use the following command:

```
./parallel_mapgen.sh --scanner pattern --count 4294967295
```

This command splits up the seed range between processes concurrently. You can adjust the number of threads with --threads.

### Options

- `--scanner <type>`: How to analyze levels.
- `--start <offset>`: The seed to start from.
- `--count <number_of_seeds>`: The number of seeds to process.
- `--threads <number_of_processes>`: The number of concurrent processes (default is one less then CPU threads)

## Terminology

These terms clarify the different types of seeds used in Diablo MapGen and provide a clearer understanding of their roles in the game:

- **Game Seed:** The time() value set on game creation. It serves as the initial seed for generating dungeon seeds and determines the overall characteristics of a game session.
- **Dungeon Seed:** The floor-specific seed saved to the seed table. This seed determines the placement of monsters and items within each floor of the dungeon. The dungeon seed is derived from the game seed at the beginning of the game session.
- **Level Seed:** This is the value that generates the specific layout of tiles for a given level in the dungeon. The level seed is derived by starting with the dungeon seed and then letting the RNG advance each time the level generator does not produce a valid level.

## Additional tools

`seed_table` shows what dungeon seeds correspond to a given game seed and when that game occurred (as game seeds are a unix timestamp representing the local date/time), or show what game seeds lead to a given dungeon seed. This allows expanding an ideal dungeon seed into a complete dungeon table to evaluate the overall run.

`sort_candidates` is mainly intended for analysis, if you have a list of dungeon seeds and you want to find out which ones occurred in games started near a certain point of time you can use this tool to sort by proximity to a unix timestamp.
