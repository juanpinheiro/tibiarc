# Recordings and versions

## Formats

| Program | Extension | Says its game version |
| --- | --- | --- |
| TibiaCAM | `.cam` | yes |
| Tibiacast | `.recording` | yes |
| TibiaMovie | `.tmv`, `.tmv2` | yes |
| TibiaReplay | `.trp` | yes |
| TibiaTimeMachine | `.ttm` | yes |
| TibiCAM | `.rec` | no |
| YATC | `.yatc` | no |

`.byn` files can be converted to `.ttm` with
[TibiaTimeMachine](https://github.com/tulio150/tibia-ttm/).

For formats that don't say their version, the web player takes it from the
`Tibia.dat` it is given; the command-line tools need `--input-version`.
Some releases shipped identical data files (7.40 and 7.41, for instance), in
which case the earliest is assumed and can be corrected on the page.

Damaged recordings are played as far as they can be read.

## Game versions

The protocol of Tibia 7.11 to 8.62 is well covered; earlier and later
versions may be hit or miss. Differences between versions live in one place,
`core/assets/versions.cpp`.

Whatever the version of the recording, the interface around the game window
is the 7.7 client's. Things later protocols added that it has no place for
are not shown; things earlier ones lacked are shown empty.

## What has been verified

| | State |
| --- | --- |
| 8.40 `.tmv2`, 8.50 `.yatc` (samples in `tests/`) | Parsed and painted by the test suite |
| 7.40 `.rec` | Played by hand with the 7.40 data files; no sample in the repository |
| Encrypted `.rec` (later TibiCAM versions) | Implemented without external libraries, not yet exercised |

Recordings that fail to play are valuable: a sample that can be shared makes
the fix testable.

## Known gaps in the interface

- The console starts on the default channel, since a recording doesn't tell
  which one the player was looking at; timestamps are not shown.
- Only part of the interface reacts to the mouse: console tabs, the
  Skills, Battle and VIP buttons, closing windows of the sidebar, and the
  wheel over lists and console text. Scrollbars can't be dragged.
- No trade window, no minimised windows, no automap marks, no VIP status
  icons.
- The Skills and Battle windows are placed below the containers when there
  is room, rather than where the player had them.
