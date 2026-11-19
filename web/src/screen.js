/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright 2026 "Juan Pinheiro"
 * This file is part of tibiarc. */

/* Presents the engine's picture on a canvas that follows the size of its
 * container, one canvas pixel per CSS pixel as in the game client, passes
 * clicks on to the interface, and lets the bar between game window and
 * console be dragged. */

const MINIMUM_WIDTH = 550;
const MINIMUM_HEIGHT = 450;
const SIDEBAR_WIDTH = 176;

export class Screen {
  constructor(engine, canvas, container) {
    this.engine = engine;
    this.canvas = canvas;
    this.container = container;
    this.context = canvas.getContext("2d", { alpha: false });
    this.drag = null;
    this.scrolling = false;

    this.observer = new ResizeObserver(() => this.fit());
    this.observer.observe(container);

    canvas.addEventListener("pointerdown", (event) => this.#press(event));
    canvas.addEventListener("pointermove", (event) => this.#move(event));
    canvas.addEventListener("pointerup", (event) => this.#release(event));
    canvas.addEventListener("pointercancel", (event) => this.#release(event));
    canvas.addEventListener("click", (event) =>
      this.engine.click(event.offsetX, event.offsetY));
    canvas.addEventListener("wheel", (event) => this.#wheel(event),
      { passive: false });
  }

  static size(container) {
    return {
      width: Math.max(MINIMUM_WIDTH, Math.floor(container.clientWidth)),
      height: Math.max(MINIMUM_HEIGHT, Math.floor(container.clientHeight)),
    };
  }

  fit() {
    const { width, height } = Screen.size(this.container);

    if (width !== this.canvas.width || height !== this.canvas.height) {
      this.canvas.width = width;
      this.canvas.height = height;
      this.engine.resize(width, height);
    }
  }

  paint() {
    const picture = this.engine.render();

    if (picture) {
      this.context.putImageData(picture, 0, 0);
    }
  }

  dispose() {
    this.observer.disconnect();
  }

  #overSplitter(event) {
    const { top, height } = this.engine.splitter;

    return event.offsetX < this.canvas.width - SIDEBAR_WIDTH &&
      event.offsetY >= top - 1 && event.offsetY < top + height + 1;
  }

  /* The page only scrolls when nothing under the pointer does. */
  #wheel(event) {
    const notches = Math.sign(event.deltaY);

    if (this.engine.scroll(event.offsetX, event.offsetY, notches)) {
      event.preventDefault();
    }
  }

  #press(event) {
    if (this.#overSplitter(event)) {
      this.drag = { y: event.offsetY, height: this.engine.consoleHeight };
      this.canvas.setPointerCapture(event.pointerId);
    } else if (this.engine.press(event.offsetX, event.offsetY)) {
      this.scrolling = true;
      this.canvas.setPointerCapture(event.pointerId);
    }
  }

  #move(event) {
    if (this.scrolling) {
      this.engine.drag(event.offsetX, event.offsetY);
    }

    if (this.drag) {
      this.engine.consoleHeight = this.drag.height + this.drag.y - event.offsetY;
    }

    const splitting = this.drag !== null || this.#overSplitter(event);

    this.canvas.classList.toggle("splitting", splitting);
    this.canvas.classList.toggle(
      "pressable",
      !splitting && this.engine.clickable(event.offsetX, event.offsetY));
  }

  #release(event) {
    if (this.scrolling) {
      this.engine.release(event.offsetX, event.offsetY);
    }

    this.drag = null;
    this.scrolling = false;
  }
}
