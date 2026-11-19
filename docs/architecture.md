# Architecture

## Layers

```
apps/  platform/wasm/        entry points: command line, WebAssembly
        │
        ▼
     client/                 game state → the picture the client showed
        │
        ▼
      core/                  recording → events → game state
```

Dependencies point downwards only. `core` and `client` perform no I/O and
have no platform dependencies: they are handed buffers and paint into a
canvas in memory. Reading files, the browser and the command line belong to
the entry points.

## Folder map

```
core/
  common/        Byte readers, errors, small utilities, crypto
  assets/        Tibia.dat / .spr / .pic and per-version differences
  graphics/      Software canvas and sprites
  game/          The state of the game world (Gamestate)
  protocol/      Network protocol parser and the events it produces
  recording/     Container formats
client/
  playback/      Session: a recording being played, seeking
  world/         What the client remembers beyond the game state:
                 speech, console, automap, battle and VIP lists
  scene/         The game window: map, light, creatures, overlays
  resources/     Fonts and interface pieces cut from Tibia.pic
  ui/            The window around the game view
    console/       Parts of the console panel
    sidebar/       Panels and windows of the sidebar
  viewer.hpp     Facade: one recording, one window
platform/wasm/   C interface of the viewer
web/             Browser front end
apps/
  render/        Frames as PNG or raw video
  miner/         Events as JSON
  collator/      Collection maintenance
  shared/        Command-line parsing, file mapping, PNG writer
tests/           Unit tests and sample recordings
third_party/     Vendored dependencies (LZMA, JSON)
cmake/           Shared build settings
scripts/         Maintenance scripts
```

## Data flow

```
recording file ──► recording/ ──► protocol/Parser ──► Events
                                                        │ Update()
                                                        ▼
                                      Gamestate  ◄── Session.Seek(time)
                                          │               │ Apply()
                                          │               ▼
                                          │             World
                                          ▼               │
                                   scene/ + ui/  ◄────────┘
                                          │
                                          ▼
                                       Canvas ──► PNG / <canvas>
```

1. A reader in `core/recording` unwraps the container into timestamped
   packets, which `protocol/Parser` turns into a list of events per frame.
   This happens once, when the recording is opened.
2. `Session::Seek` applies the events up to the wanted time to `Gamestate`,
   and hands each to `World`, which keeps what the game state has no place
   for. Seeking backwards replays from the start.
3. `Viewer::Render` paints the window from `Gamestate` and `World`.

## Rules

- **Frames are pure.** A frame is a function of the recording and a time in
  milliseconds. Painting never changes state, and state never depends on how
  often or in what order frames were painted.
- **`client` only reads `Gamestate`.** Anything it needs to remember is fed
  through `World::Apply`.
- **Version differences stay in `core/assets/versions.cpp`**, expressed as
  features and translation tables, never as version checks scattered through
  the code.
- **The picture matches the original client.** Positions, colours, timings
  and rounding are the client's; see [Rendering](rendering.md).
- **Include paths** are relative to `core/` or `client/`; the two trees
  don't share folder names.

## Conventions

Types, methods and members are `PascalCase`, locals `camelCase`; namespaces
`trc` and `trc::Client`. Formatting follows `.clang-format`
(`scripts/format.sh`). New files carry the license header of their
neighbours and are added to the sorted source list of their folder's
`CMakeLists.txt`. Builds are warning-free with `-Wall -Wextra`.
