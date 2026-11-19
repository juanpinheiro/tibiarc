# tibiarc

`tibiarc` plays back recordings of old Tibia sessions the way the game client
of the time showed them: the game window scaled smoothly to any size with
names, health bars, speech and damage numbers drawn crisp on top of it, framed
by the client's own interface. It runs in the browser as a WebAssembly module
and needs no game client.

It reads recordings from **Tibia 7.11 to 8.62** (other versions may work) as
long as the `Tibia.dat`, `Tibia.spr` and `Tibia.pic` of that version are at
hand. The interface is the one of the 7.7 client, so use a `Tibia.pic` from
that era whatever the version of the recording.

This is a fork of [tibiarc by John Högberg](https://github.com/tibiacast/tibiarc),
whose parser and recording readers still are the heart of it.

## Supported recording formats

| Program | Extension |
| --- | --- |
| TibiaCAM | `.cam` |
| Tibiacast | `.recording` |
| TibiaMovie | `.tmv`, `.tmv2` |
| TibiaReplay | `.trp` |
| TibiaTimeMachine | `.ttm` |
| TibiCAM | `.rec` |
| YATC | `.yatc` |

`.byn` files can be converted to `.ttm` with
[TibiaTimeMachine](https://github.com/tulio150/tibia-ttm/).

## Layout of the repository

```
core/            Recording → game state. No I/O, no platform dependencies.
  common/          Byte readers, errors, small utilities
  assets/          Tibia.dat / .spr / .pic and per-version differences
  graphics/        Software canvas and sprites
  game/            The state of the game world
  protocol/        Network protocol parser and the events it produces
  recording/       Container formats
client/          Game state → the picture the client showed
  playback/        A recording being played: seeking, time
  world/           What the client remembers: speech, console, VIP list
  scene/           The game window: map, light, creatures, overlays
  resources/       Fonts and interface pieces cut from Tibia.pic
  ui/              The window around the game view: console, sidebar
  viewer.hpp       Facade: one recording, one window
platform/wasm/   C interface of the viewer for WebAssembly
web/             Browser front end (plain ES modules, no build step)
apps/            Command-line tools
  render/          Writes frames of a recording as PNG
  miner/           Dumps the events of a recording as JSON
  collator/        Sorts and deduplicates collections of recordings
tests/           Unit tests and sample recordings
third_party/     Vendored dependencies
```

Dependencies point one way: `apps`, `platform` → `client` → `core`.

## Building

Requirements: CMake 3.25+, Ninja (or Make) and a C++23 compiler. zlib is
optional; without it `.tmv`, `.tmv2` and `.recording` files cannot be read.

### Command-line tools

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

```sh
# Three frames, ten seconds apart, of a 800×600 window
build/apps/render/tibiarc-render --size 800 600 --start 30000 --step 10000 \
    --count 3 path/to/7.70 recording.rec frame-
```

The first argument of every tool is a folder holding the three data files of
the recording's version.

### Web player

With the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
activated:

```sh
emcmake cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
```

This places the module in `web/wasm/`. Serve `web/` with any static file
server and open it:

```sh
python3 -m http.server --directory web 8000
```

Drop a recording and the data files of its version on the page. Nothing is
uploaded; the data files are remembered in the browser per game version, so
afterwards the recording alone is enough. Space pauses, the arrow keys skip,
and the bar between game window and console can be dragged as in the client.

## Testing

```sh
cmake -S . -B build -G Ninja -DTIBIARC_TEST_DATA=/path/to/data
cmake --build build
ctest --test-dir build
```

`TIBIARC_TEST_DATA` is a folder with one sub-folder of data files per version
(`8.40/Tibia.dat`, …). Without it only the unit tests run, as the game's files
cannot be distributed with the project.

## Thanks

To John Högberg and Tibiacast for tibiarc itself; to
[gurka](https://github.com/gurka) for reverse-engineering the TibiCAM `.rec`
format; to [gesior](https://github.com/gesior) for maintaining a collection of
data files of old versions; and to the TibiaBR staff for their public archive
of recordings.

## License

AGPL-3.0-or-later, see [COPYING](COPYING). The sample recordings in `tests/`
keep the licenses of their authors, see [tests/LICENSES.md](tests/LICENSES.md).
