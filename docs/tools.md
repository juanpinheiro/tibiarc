# Command-line tools

All tools take the folder with the data files of the recording's version as
their first argument (see [Getting started](getting-started.md)) and accept
`--help`.

## tibiarc-render

Paints frames of a recording exactly as the web player would, without a
browser.

```
tibiarc-render [OPTION...] data_folder input_path output_prefix
```

| Option | Meaning |
| --- | --- |
| `--size W H` | Window size in pixels (default 800×600, minimum 550×450) |
| `--start MS` | Time of the first frame |
| `--step MS` | Time between frames (default 1000) |
| `--fps RATE` | Frames per second, instead of `--step` |
| `--count N` | Number of frames (default 1) |
| `--click X Y` | Press a point of the window first, as a viewer could; may be repeated |
| `--raw` | Write packed 24-bit RGB frames to standard output instead of PNG files |
| `--discard` | Paint without writing; used by the tests |
| `--input-version X.Y` | Game version, when the recording doesn't say |

Stills, written as `frame-0000.png`, `frame-0001.png`, …:

```sh
tibiarc-render --size 800 600 --start 30000 --step 10000 --count 3 \
    ~/tibia-data/7.70 recording.rec frame-
```

A video, by piping raw frames into ffmpeg:

```sh
tibiarc-render --size 1280 720 --fps 30 --count 900 --raw \
    ~/tibia-data/7.70 recording.rec - |
  ffmpeg -f rawvideo -pix_fmt rgb24 -s 1280x720 -r 30 -i - out.mp4
```

`--count` is the length: frames = seconds × fps. The runtime of the recording
is printed on standard error.

## miner

Dumps what happens in a recording as JSON: one entry per frame with its
timestamp and events.

```
miner [OPTION...] data_folder input_path
```

| Option | Meaning |
| --- | --- |
| `--start-time MS`, `--end-time MS` | Only this part of the recording |
| `--input-format F` | `cam`, `rec`, `tibiacast`, `tmv1`, `tmv2`, `trp`, `ttm`, `yatc`; guessed otherwise |
| `--input-version X.Y` | Game version, when the recording doesn't say |
| `--skip-terrain`, `--skip-creature-presence`, `--skip-creature-updates`, `--skip-effects`, `--skip-inventory`, `--skip-messages`, `--skip-player-updates` | Leave out a kind of event |
| `--terrain-only` | Only map events, for map tracking |
| `--dry-run` | Parse everything, print nothing |

## collator

Sorts a pile of recordings into a collection: validates each file, works out
its version and drops duplicates by checksum.

```
collator [OPTION...] collection_root source
```

| Option | Meaning |
| --- | --- |
| `--move` | Move files instead of copying |
| `--deny-list FILE` | Skip files whose checksums are listed |
| `--verbose` | Print what is done with every file |
| `--dry-run` | Change nothing |
