# tibiarc

Plays back old Tibia recordings (7.11–8.62) as the 7.7 game client showed
them. C++23 core and client compiled to WebAssembly, plain-JS web front end.
See README.md for the folder map and build commands.

## Commands

```sh
export PATH=$HOME/.local/bin:$PATH            # cmake, ninja
cmake -S . -B build -G Ninja -DTIBIARC_TEST_DATA=$HOME/tibia-data
ninja -C build && ctest --test-dir build      # native build + tests

source ~/emsdk/emsdk_env.sh                   # web build → web/wasm/
emcmake cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build-wasm
```

Look at what the client paints without a browser:

```sh
build/apps/render/tibiarc-render --input-version 8.50 --size 800 600 \
    --start 60000 --count 1 ~/tibia-data/8.50 tests/samples/8.50/sample.yatc /tmp/f
```

Game data files live outside the repository, one folder per version
(`~/tibia-data/<version>/Tibia.{dat,spr,pic}`); they must never be committed.

## Architecture rules

- Dependencies point one way: `apps`, `platform` → `client` → `core`. `core`
  and `client` perform no I/O and have no platform dependencies; they must
  build with Emscripten.
- `core` turns a recording into events and applies them to `Gamestate`.
  `client` only reads `Gamestate`; anything the client remembers beyond it
  (speech, console, VIP list) lives in `client/world` and is fed by
  `World::Apply`.
- Every frame is a pure function of the recording and a time in
  milliseconds: state must never depend on how often or in what order frames
  were painted. Animation is derived from the time through the 100 ms tick
  clock in `client/scene/animation.hpp`.
- The map is composed at 480×352, scaled bilinearly to the game view, and
  everything on top of it (names, bars, speech, numbers) is drawn afterwards
  in window pixels, never scaled. Interface pieces are tiled, never stretched.
- The picture must match the original 7.7 client pixel for pixel. Do not
  "improve" positions, colours, timings or rounding; when unsure how the
  client behaved, find out instead of guessing.
- Include paths are relative to `core/` or `client/` (`"game/gamestate.hpp"`,
  `"scene/mapview.hpp"`); the two trees must not share folder names.

## Conventions

- Repository content is English. Follow `.clang-format` (4 spaces, 80
  columns); `scripts/format.sh` formats everything.
- Types, methods and members are `PascalCase`, locals `camelCase`. Namespaces
  `trc` (core) and `trc::Client` (client).
- New files start with the AGPL header of their neighbours and are added to
  the sorted source list of their folder's `CMakeLists.txt`.
- Comments say what the client does and why, in present tense. Named
  constants instead of bare numbers.
- Builds are warning-free with `-Wall -Wextra`.
- Web code is dependency-free ES modules, one responsibility per file in
  `web/src/`.
