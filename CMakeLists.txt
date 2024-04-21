cmake_minimum_required(VERSION 3.22)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(WARNING [[In-source build detected, please eg. create a new directory and use `cmake ..`]])
endif()

set(BIN_TARGET diablo-mapgen)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/parallel_mapgen.sh ${CMAKE_CURRENT_BINARY_DIR}/parallel_mapgen.sh COPYONLY)

project(DiabloMapGen
  VERSION "0.1.0"
  LANGUAGES C CXX)
set(PROJECT_VERSION_WITH_SUFFIX "${VERSION_PREFIX}${VERSION_SUFFIX}")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions -funroll-loops -march=native -fsigned-char")
  if(CMAKE_BUILD_TYPE MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  endif()
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /GL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
endif()

set(CMAKE_CXX_STANDARD 20)

add_executable(${BIN_TARGET}
  Source/drlg_l1.cpp
  Source/drlg_l2.cpp
  Source/drlg_l3.cpp
  Source/drlg_l4.cpp
  Source/engine.cpp
  Source/analyzer/gameseed.cpp
  Source/analyzer/path.cpp
  Source/analyzer/pattern.cpp
  Source/analyzer/puzzler.cpp
  Source/analyzer/quest.cpp
  Source/analyzer/scannerName.cpp
  Source/analyzer/stairs.cpp
  Source/analyzer/warp.cpp
  Source/asset/asset.cpp
  Source/asset/globalcache.cpp
  Source/asset/diabdat.cpp
  Source/asset/diabdat/data.cpp
  Source/asset/diabdat/levels.cpp
  Source/asset/diabdat/levels/l1data.cpp
  Source/asset/diabdat/levels/l2data.cpp
  Source/asset/diabdat/levels/l3data.cpp
  Source/asset/diabdat/levels/l4data.cpp
  Source/asset/diabdat/levels/towndata.cpp
  Source/asset/diabdat/plrgfx.cpp
  Source/asset/hellfire.cpp
  Source/asset/hellfire/nlevels.cpp
  Source/asset/hellfire/nlevels/l5data.cpp
  Source/asset/hellfire/nlevels/l6data.cpp
  Source/asset/hellfire/nlevels/towndata.cpp
  Source/funkMapGen.cpp
  Source/gendung.cpp
  Source/itemdat.cpp
  Source/items.cpp
  Source/level.cpp
  Source/lighting.cpp
  Source/mapGen/configuration.cpp
  Source/monstdat.cpp
  Source/monster.cpp
  Source/objdat.cpp
  Source/objects.cpp
  Source/path.cpp
  Source/quests.cpp
  Source/setmaps.cpp
  Source/spelldat.cpp
  #Source/stores.cpp
  Source/themes.cpp
  Source/trigs.cpp
  )

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_BUILD_TYPE MATCHES "Debug")
	target_link_libraries(${BIN_TARGET} PUBLIC "-fsanitize=undefined")
endif()

if(CMAKE_STRIP)
  add_custom_command(
    TARGET ${BIN_TARGET} POST_BUILD
    DEPENDS ${BIN_TARGET}
    COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:${CMAKE_STRIP}>
    ARGS $<TARGET_FILE:${BIN_TARGET}>)
endif()

add_executable (seed_table "tools/seed_table.cpp")
target_compile_features(seed_table PUBLIC cxx_std_20)

add_executable (sort_candidates "tools/sort_candidates.cpp")
target_compile_features(sort_candidates PUBLIC cxx_std_20)
