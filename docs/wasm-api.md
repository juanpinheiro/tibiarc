# WebAssembly interface

`platform/wasm/api.cpp` exposes the viewer as plain C functions operating on
one player — a page shows one recording at a time. `web/src/engine.js` wraps
them in a class; use that unless you need something else.

## From JavaScript

```js
import { Engine, OpenResult } from "./src/engine.js";

const engine = await Engine.load();

const version = engine.probeVersion(name, recordingBytes);   // 770, or 0

const result = engine.open({
  name, version,                    // 0 = take it from the recording
  recording, dat, spr, pic,         // Uint8Array each
  width: 800, height: 600,
});

if (result === OpenResult.Failed) throw new Error(engine.lastError);

engine.seek(12_000);                // milliseconds
context.putImageData(engine.render(), 0, 0);
```

| Member | |
| --- | --- |
| `runtime` | Length of the recording in milliseconds |
| `seek(ms)` | Move to a time; `false` on failure |
| `render()` | `ImageData` of the window, valid until the next call into the engine |
| `resize(w, h)` | Change the window size (minimum 550×450) |
| `consoleHeight` | Get or set the height of the console |
| `click(x, y)` | Press a point of the window; `true` if something was there |
| `clickable(x, y)` | Whether pressing there does something |
| `splitter` | `{ top, height }` of the bar between game window and console |
| `close()` | Release the recording |

`open` returns `OpenResult.Complete`, `Damaged` (plays what could be read) or
`Failed`.

## C functions

| Function | |
| --- | --- |
| `trc_alloc(size)`, `trc_free(ptr)` | Buffers for handing files over |
| `trc_probe_version(name, data, size)` | Version a recording names, as `major * 100 + minor`, or 0 |
| `trc_identify_data(data, size)` | Version a `Tibia.dat` belongs to, same encoding |
| `trc_open(name, version, pic, n, spr, n, dat, n, rec, n, w, h)` | 1 ok, 2 damaged, 0 failed. Takes ownership of the four buffers |
| `trc_close()` | |
| `trc_error()` | Text of the last failure |
| `trc_runtime()`, `trc_now()` | Milliseconds |
| `trc_seek(ms)` | 1 on success |
| `trc_resize(w, h)`, `trc_width()`, `trc_height()` | |
| `trc_set_console_height(h)`, `trc_console_height()` | |
| `trc_click(x, y)`, `trc_clickable(x, y)` | Press a point of the window as last painted; 1 if something is there |
| `trc_splitter()` | Top in the high 16 bits, height in the low |
| `trc_render()` | Pointer to RGBA pixels, `width × height × 4` bytes |

The module also runs under Node.js, which is handy for scripted checks.
