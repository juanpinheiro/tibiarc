# Recordings and versions

## Formats

| Program | Extension | Says its game version |
| --- | --- | --- |
| TibiaCAM | `.cam` | yes |
| Tibiacast | `.recording` | yes |
| TibiaMovie | `.tmv`, `.tmv2` | yes |
| TibiaReplay | `.trp` | yes |
| TibiaTimeMachine | `.ttm` | yes |
| TibiCAM | `.rec` | guessed from its contents |
| YATC | `.yatc` | no — pass it |

`.byn` files can be converted to `.ttm` with
[TibiaTimeMachine](https://github.com/tulio150/tibia-ttm/).

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
| 7.x recordings | Supported by the parser, not covered by a sample |
| `.rec` decryption | Implemented without external libraries, not covered by a sample |

Recordings that fail to play are valuable: a sample that can be shared makes
the fix testable.

## Known gaps in the interface

- The console always shows the default channel; timestamps are not shown.
- No trade window, no minimised windows, no automap marks, no VIP status
  icons.
- The Skills and Battle windows are placed below the containers when there
  is room, rather than where the player had them.
