# Web player

A static page in `web/` that plays recordings entirely in the browser. Build
the WebAssembly module first, see [Getting started](getting-started.md).

## Opening a recording

Drop (or pick) the recording together with the `Tibia.dat`, `Tibia.spr` and
`Tibia.pic` of its version. The page shows which of the four it still misses.

- The game version is read from the recording. For formats that don't record
  it (`.rec`, `.yatc`) it is taken from the `Tibia.dat`, and can be corrected.
- Nothing is uploaded; the files are read by the browser.
- The data files are remembered on the device, one set per game version
  (IndexedDB). Next time, a recording of a known version plays on its own.

## Controls

| Control | Action |
| --- | --- |
| Play button, `Space` | Pause and resume; at the end, start over |
| Seek bar | Jump anywhere |
| `←` / `→` | Skip five seconds back / forward |
| Speed | ¼× to 64× |
| Fullscreen | Player fills the screen |
| Drag the bar between game window and console | Resize the console, as in the client |
| Click a tab of the console | Bring that channel to the front |
| Click Skills / Battle / VIP | Open or close that window, if there is room for it |
| Click the × of a window in the sidebar | Close it; a container shows again the next time the recording opens it |
| Open another | Back to the file page |

The window follows the size of the page, one canvas pixel per CSS pixel, and
is never smaller than the client's minimum of 550×450. The game view keeps
the aspect of the map and takes the room the console leaves it.

## Files

| File | Responsibility |
| --- | --- |
| `index.html`, `styles.css` | The page |
| `src/main.js` | Wires the page together |
| `src/engine.js` | The WebAssembly module as an object |
| `src/intake.js` | Collects the recording and its data files |
| `src/library.js` | Remembers data files per version |
| `src/transport.js` | Playback clock: play, pause, speed, seek |
| `src/screen.js` | Canvas presentation and the draggable splitter |

To embed the player elsewhere, `engine.js` is all that is needed; see
[WebAssembly interface](wasm-api.md).
