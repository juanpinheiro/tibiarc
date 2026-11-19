/* The WebAssembly player behind a small object interface. One engine shows
 * one recording at a time. */

import createModule from "../wasm/tibiarc.mjs";

export const OpenResult = Object.freeze({ Failed: 0, Complete: 1, Damaged: 2 });

export class Engine {
  static async load() {
    return new Engine(await createModule());
  }

  constructor(module) {
    this.module = module;
  }

  /* Copies a file into the module's memory, which then owns it. */
  #give(bytes) {
    const address = this.module._trc_alloc(bytes.length);

    this.module.HEAPU8.set(bytes, address);

    return address;
  }

  #withName(name, action) {
    const address = this.module.stringToNewUTF8(name);

    try {
      return action(address);
    } finally {
      this.module._free(address);
    }
  }

  get lastError() {
    return this.module.UTF8ToString(this.module._trc_error());
  }

  /* The game version a recording names, as major * 100 + minor, or 0. */
  probeVersion(name, bytes) {
    const address = this.#give(bytes);

    try {
      return this.#withName(name, (text) =>
        this.module._trc_probe_version(text, address, bytes.length));
    } finally {
      this.module._trc_free(address);
    }
  }

  open({ name, version, recording, dat, spr, pic, width, height }) {
    return this.#withName(name, (text) =>
      this.module._trc_open(
        text, version,
        this.#give(pic), pic.length,
        this.#give(spr), spr.length,
        this.#give(dat), dat.length,
        this.#give(recording), recording.length,
        width, height));
  }

  close() {
    this.module._trc_close();
  }

  get runtime() {
    return this.module._trc_runtime();
  }

  seek(time) {
    return this.module._trc_seek(time) !== 0;
  }

  resize(width, height) {
    this.module._trc_resize(width, height);
  }

  /* The draggable bar between the game window and the console. */
  get splitter() {
    const packed = this.module._trc_splitter();

    return { top: packed >>> 16, height: packed & 0xffff };
  }

  get consoleHeight() {
    return this.module._trc_console_height();
  }

  set consoleHeight(height) {
    this.module._trc_set_console_height(height);
  }

  /* Paints the window and returns it as an ImageData that is only valid
   * until the next call into the engine. */
  render() {
    const address = this.module._trc_render();

    if (address === 0) {
      return null;
    }

    const width = this.module._trc_width();
    const height = this.module._trc_height();
    const pixels = new Uint8ClampedArray(
      this.module.HEAPU8.buffer, address, width * height * 4);

    return new ImageData(pixels, width, height);
  }
}
