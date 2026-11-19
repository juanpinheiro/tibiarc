# tibiarc

Watch recordings of old Tibia sessions the way the game client of the time
showed them — in the browser, with no game client installed.

The game window is scaled smoothly to any size, while names, health bars,
speech and damage numbers stay crisp on top of it, framed by the client's own
console and sidebar.

- **Versions:** recordings from Tibia 7.11 to 8.62 (others may work)
- **Formats:** `.cam`, `.rec`, `.recording`, `.tmv`, `.tmv2`, `.trp`, `.ttm`, `.yatc`
- **Runs as:** a WebAssembly module with a plain-JavaScript page, or
  command-line tools

You bring the recording and the `Tibia.dat`, `Tibia.spr` and `Tibia.pic` of
its game version; the project ships none of the game's files.

## Quick start

```sh
# Web player (needs the Emscripten SDK activated)
emcmake cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
python3 -m http.server --directory web 8000     # open http://localhost:8000

# Command-line tools
cmake -S . -B build -G Ninja
cmake --build build
build/apps/render/tibiarc-render --size 800 600 --start 30000 \
    path/to/data recording.rec frame-
```

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
