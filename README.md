# tibiarc

**Old Tibia recordings, played back exactly as the game client showed them —
in your browser, at any size, with nothing to install.**

<img width="1919" height="945" alt="A Tibia 7.4 recording playing in the browser, with the client's console, inventory, backpack, skills and battle list" src="https://github.com/user-attachments/assets/5a63a870-319c-4224-88b1-7531ab891047" />

Drop a `.rec`, `.cam` or any other supported recording on the page and you
are back in the client of the time: the game window, the console with its
channels, the inventory, your backpacks, skills, battle list and VIP list.
Pause, seek anywhere, watch at up to 64× speed.

## Why it looks right

Most ways of watching old recordings either need the original client or
render the whole picture small and blow it up, which turns names and speech
into mush. tibiarc does what the client did:

- **A sharp picture at any size.** The map is composed at its native
  resolution and scaled smoothly; names, health bars, speech and damage
  numbers are then drawn on top at their own size, so text stays crisp from
  a 550-pixel window to fullscreen.
- **The real interface.** Console, sidebar, frames and fonts are assembled
  from the client's own interface pieces, laid out with its measurements.
- **The client's timing.** Walking, animations, effects, flying missiles and
  the fading of daylight follow the client's 100 ms clock.
- **The client's light.** Torches, spells and the day cycle light the map
  with smooth gradients, per field, in colour.

## What you get

| | |
| --- | --- |
| **Plays in the browser** | A 300 KB WebAssembly module and a page with no dependencies. Nothing is uploaded; your files never leave your machine. |
| **The whole client window** | Game view, console with channel tabs and private conversations, automap, health and mana, inventory, containers, skills, battle list, VIP list. |
| **It reacts** | Switch console tabs, open and close Skills, Battle and VIP, close windows, scroll backpacks and chat history with the wheel or the scrollbars, drag the bar between map and console. |
| **Seek anywhere** | Jump to any moment of a 45-minute hunt instantly; speeds from ¼× to 64×. |
| **Knows your files** | The game version is read from the recording or recognised from `Tibia.dat`; data files are remembered per version, so next time the recording alone is enough. |
| **Makes videos** | A command-line renderer paints the same frames without a browser and streams them into ffmpeg at any resolution and frame rate. |
| **Eight formats, 7.11 to 8.62** | `.cam`, `.rec`, `.recording`, `.tmv`, `.tmv2`, `.trp`, `.ttm`, `.yatc`. |
| **No dependencies** | C++23 and CMake. No Qt, no OpenSSL, no libav. |

You bring the recording and the `Tibia.dat`, `Tibia.spr` and `Tibia.pic` of
its game version; the project ships none of the game's files.

## Quick start

```sh
# Web player (needs the Emscripten SDK activated)
emcmake cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
python3 -m http.server --directory web 8000     # open http://localhost:8000
```

Drop your recording and the three data files on the page, press Play.

```sh
# Command-line tools
cmake -S . -B build -G Ninja
cmake --build build

# A 30-second 720p video
build/apps/render/tibiarc-render --size 1280 720 --fps 30 --count 900 --raw \
    path/to/data recording.rec - |
  ffmpeg -f rawvideo -pix_fmt rgb24 -s 1280x720 -r 30 -i - out.mp4
```

## What changed from the original tibiarc

This fork keeps the original's protocol parser and recording readers and
rebuilds the rest around them:

- **New renderer.** The original drew everything into one small picture and
  scaled it; this one separates the map from what is drawn over it, and adds
  the client's lighting, draw order and animation timing.
- **The client's interface**, which the original didn't draw: console,
  sidebar windows, automap, and interaction with them.
- **A web player** in place of the desktop one: WebAssembly core, static
  page, files remembered in the browser.
- **A smaller footprint.** The desktop toolkit, the video encoding library
  and the cryptography library are gone; decryption and hashing are built
  in, and video goes through a pipe to ffmpeg.
- **A layered code base**: `core` (recording → game state), `client` (game
  state → picture), and thin entry points for the web and the command line,
  each documented and tested.

## Documentation

| | |
| --- | --- |
| [Getting started](docs/getting-started.md) | Requirements, game data files, building |
| [Web player](docs/web-player.md) | Using the page, controls, what is stored |
| [Command-line tools](docs/tools.md) | `tibiarc-render`, `miner`, `collator`, making videos |
| [Recordings and versions](docs/recordings.md) | Supported formats and versions, known gaps |
| [Architecture](docs/architecture.md) | Folder map, layers, how data flows |
| [Rendering](docs/rendering.md) | How a frame is put together, timing |
| [WebAssembly interface](docs/wasm-api.md) | Embedding the player in another page |
| [Testing](docs/testing.md) | Running and extending the tests |

## Credits

tibiarc was written at Tibiacast and released as open source by
[John Högberg](https://github.com/tibiacast/tibiarc), whose protocol parser
and recording readers remain the heart of it. This fork, by
[Juan Pinheiro](https://github.com/juanpinheiro), restructures the project
and adds the client that reproduces the game window and interface, and the
web player. [AUTHORS.md](AUTHORS.md) lists who holds copyright on what.

Thanks also to [gurka](https://github.com/gurka) for reverse-engineering the
TibiCAM `.rec` format, to [gesior](https://github.com/gesior) for maintaining
a collection of data files of old versions, and to the TibiaBR staff for
their public archive of recordings.

## License

Copyright © 2011–2026 the tibiarc authors.

tibiarc is free software under the GNU Affero General Public License, version
3 or later; see [COPYING](COPYING). If you run a modified version as a
network service, the license asks you to offer its source to the users of
that service. The sample recordings in `tests/` keep the licenses of their
authors, see [tests/LICENSES.md](tests/LICENSES.md).

Tibia is a trademark of CipSoft GmbH; this project is not affiliated with or
endorsed by CipSoft and ships none of the game's files.
