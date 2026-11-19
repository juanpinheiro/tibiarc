# Testing

```sh
cmake -S . -B build -G Ninja -DTIBIARC_TEST_DATA=/path/to/tibia-data
cmake --build build
ctest --test-dir build
```

## What runs

| Test | Needs data files | Checks |
| --- | --- | --- |
| `unit` | no | Pure logic of the client: window layout, the tick clock, walking |
| `parse: <version>/<sample>` | yes | The whole recording parses |
| `render WxH: <version>/<sample>` | yes | Frames across the recording paint, at the smallest window and a large one |

`TIBIARC_TEST_DATA` is a folder with one sub-folder of data files per version
(`8.40/Tibia.dat`, …; see [Getting started](getting-started.md)). Tests of a
sample whose version has no data files are skipped with a note at configure
time, since the game's files cannot be distributed.

The render tests exercise nearly all of the client but don't judge the
picture; look at frames with `tibiarc-render` when changing how things are
drawn.

## Adding tests

- **Unit test:** add a `TEST(Name) { EXPECT_EQ(actual, expected); }` to a
  file in `tests/unit/` (new files go into `tests/CMakeLists.txt`). The
  framework is `tests/unit/check.hpp`, a page long.
- **Sample recording:** put it in `tests/samples/<version>/`, add an
  `add_sample_tests(<version> <file>)` line to `tests/CMakeLists.txt`, and
  record its author and license in `tests/LICENSES.md`. Only add recordings
  you have permission to share.

Sanitizers: configure with `-DTIBIARC_SANITIZE=ON`.
