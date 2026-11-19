/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright 2026 "Juan Pinheiro"
 * This file is part of tibiarc. */

/* Collects what is needed to play a recording: the recording itself and the
 * three data files of its game version, the latter possibly from the
 * library of files seen before. */

import { recall } from "./library.js";

/* Data files are told by their extension, whatever they are called. */
const DATA_SLOTS = { dat: "dat", spr: "spr", pic: "pic" };

function slotOf(name) {
  const extension = name.slice(name.lastIndexOf(".") + 1).toLowerCase();

  return DATA_SLOTS[extension] ?? "recording";
}

export function formatVersion(version) {
  return `${Math.floor(version / 100)}.${String(version % 100).padStart(2, "0")}`;
}

export function parseVersion(text) {
  const match = /^\s*(\d{1,2})\.(\d{1,2})\s*$/.exec(text);

  return match ? Number(match[1]) * 100 + Number(match[2].padEnd(2, "0")) : 0;
}

export class Intake {
  constructor(engine, onChange) {
    this.engine = engine;
    this.onChange = onChange;
    this.slots = { recording: null, dat: null, spr: null, pic: null };
    /* Where the game version can come from, in order of trust: the
     * recording itself, what the user typed, the Tibia.dat that was picked. */
    this.detected = 0;
    this.entered = 0;
    this.inferred = 0;
    this.remembered = null;
  }

  get version() {
    return this.detected || this.entered || this.inferred;
  }

  /* A slot is filled by a file picked now or one remembered from before. */
  file(slot) {
    return this.slots[slot] ?? this.remembered?.[slot] ?? null;
  }

  get complete() {
    return this.version > 0 &&
      ["recording", "dat", "spr", "pic"].every((slot) => this.file(slot));
  }

  async add(fileList) {
    for (const file of fileList) {
      const slot = slotOf(file.name);
      const bytes = new Uint8Array(await file.arrayBuffer());

      this.slots[slot] = { name: file.name, bytes, fresh: true };

      if (slot === "recording") {
        this.detected = this.engine.probeVersion(file.name, bytes);
        this.entered = 0;
      } else if (slot === "dat") {
        this.inferred = this.engine.identifyData(bytes);
      }
    }

    await this.#consultLibrary();
    this.onChange();
  }

  async enterVersion(text) {
    this.entered = parseVersion(text);
    await this.#consultLibrary();
    this.onChange();
  }

  async #consultLibrary() {
    this.remembered = this.version ? await recall(this.version) : null;
  }

  /* The data files as they should be remembered for this version. */
  dataFiles() {
    const keep = ({ name, bytes }) => ({ name, bytes });

    return {
      dat: keep(this.file("dat")),
      spr: keep(this.file("spr")),
      pic: keep(this.file("pic")),
    };
  }
}
