# Getting started

## Requirements

| For | You need |
| --- | --- |
| Everything | CMake 3.25+, Ninja (or Make), a C++23 compiler (GCC 13+, Clang 17+) |
| Web player | The [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) |
| `.tmv`, `.tmv2`, `.recording` files | zlib (optional natively; bundled for the web build) |

Nothing else: there are no other library dependencies.

## Game data files

Every recording must be played with the data files of the game version it was
made with:

| File | Holds | Which one |
| --- | --- | --- |
| `Tibia.dat` | Properties of items, outfits, effects | Of the recording's version |
| `Tibia.spr` | Sprites | Of the recording's version |
| `Tibia.pic` | Fonts and interface pieces | From a 7.x client, ideally 7.7 |

The files may have any name; they are told by their extension.

The interface drawn around the game window is the one of the 7.7 client, cut
from `Tibia.pic`; pictures of slightly older clients (7.4 was
tried) lack a few pieces, which are substituted. A `.pic` from a much later
client has a different layout and will produce a garbled interface.

Keep the files in one folder per version, outside the repository:

```
tibia-data/
  7.70/Tibia.dat  Tibia.spr  Tibia.pic
  8.40/Tibia.dat  Tibia.spr  Tibia.pic
```

They are copyrighted by CipSoft and must never be committed.

## Building the command-line tools

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

The tools end up in `build/apps/`. See [Command-line tools](tools.md).

## Building the web player

With the Emscripten SDK activated in the shell (`source emsdk_env.sh`):

```sh
emcmake cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
```

This writes `tibiarc.mjs` and `tibiarc.wasm` to `web/wasm/`. The page itself
has no build step; serve the `web/` folder with any static file server:

```sh
python3 -m http.server --directory web 8000
```

See [Web player](web-player.md).

## Build options

| Option | Default | Effect |
| --- | --- | --- |
| `CMAKE_BUILD_TYPE` | `RelWithDebInfo` | Usual CMake build types |
| `TIBIARC_SANITIZE` | `OFF` | Address and undefined-behaviour sanitizers |
| `TIBIARC_COVERAGE` | `OFF` | Coverage instrumentation |
| `TIBIARC_TEST_DATA` | `tests/data` | Folder of data files for the tests, see [Testing](testing.md) |
| `BUILD_TESTING` | `ON` | Build the tests |
