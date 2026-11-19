/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright 2026 "Juan Pinheiro"
 * This file is part of tibiarc. */

import { Engine, OpenResult } from "./engine.js";
import { Intake, formatVersion } from "./intake.js";
import { remember } from "./library.js";
import { Screen } from "./screen.js";
import { Transport, formatTime } from "./transport.js";

const $ = (id) => document.getElementById(id);

const SKIP = 5000;

const app = $("app");
const notice = $("notice");
const engine = await Engine.load();
const intake = new Intake(engine, showIntake);

let session = null;

/* Intake ---------------------------------------------------------------- */

function showIntake() {
  for (const item of $("checklist").children) {
    const slot = item.dataset.slot;
    const file = intake.file(slot);
    let detail = file?.name ?? "";

    if (slot === "recording" && file && intake.version) {
      detail += ` — version ${formatVersion(intake.version)}`;
    } else if (file && !file.fresh) {
      detail = `remembered for ${formatVersion(intake.version)}`;
    }

    item.toggleAttribute("data-have", file !== null);
    item.querySelector(".detail").textContent = detail;
  }

  const input = $("version-input");

  $("version-row").hidden =
    intake.file("recording") === null || intake.detected > 0;
  $("version-hint").textContent = intake.inferred && !intake.entered
    ? `Taken from Tibia.dat: ${formatVersion(intake.inferred)}. Change it ` +
      "if the recording is from another release that shipped the same file."
    : "";

  if (document.activeElement !== input && !intake.entered) {
    input.value = intake.inferred ? formatVersion(intake.inferred) : "";
  }
  $("open-button").disabled = !intake.complete;
}

async function addFiles(files) {
  notice.textContent = "";
  await intake.add(files);
}

$("file-input").addEventListener("change", (event) => {
  /* Clearing the input, so the same file can be picked again, empties its
   * live file list. */
  addFiles([...event.target.files]);
  event.target.value = "";
});

$("dropzone").addEventListener("keydown", (event) => {
  if (event.key === "Enter" || event.key === " ") {
    $("file-input").click();
  }
});

for (const type of ["dragenter", "dragover"]) {
  document.addEventListener(type, (event) => {
    event.preventDefault();
    $("dropzone").classList.add("over");
  });
}

document.addEventListener("dragleave", () =>
  $("dropzone").classList.remove("over"));

document.addEventListener("drop", (event) => {
  event.preventDefault();
  $("dropzone").classList.remove("over");

  if (session === null) {
    addFiles([...event.dataTransfer.files]);
  }
});

$("version-input").addEventListener("input", (event) =>
  intake.enterVersion(event.target.value));

$("open-button").addEventListener("click", start);

/* Playback -------------------------------------------------------------- */

function start() {
  const recording = intake.file("recording");

  /* The stage must be laid out before its size can be measured. */
  app.dataset.state = "playing";
  $("intake").hidden = true;
  $("stage").hidden = false;

  let result = OpenResult.Failed;
  let reason = "";

  try {
    result = engine.open({
      name: recording.name,
      version: intake.version,
      recording: recording.bytes,
      dat: intake.file("dat").bytes,
      spr: intake.file("spr").bytes,
      pic: intake.file("pic").bytes,
      ...Screen.size($("screen")),
    });
    reason = engine.lastError;
  } catch (error) {
    /* The engine itself gave up, which leaves it unusable. */
    reason = `the player crashed (${error.message}). Reload the page`;
  }

  if (result === OpenResult.Failed) {
    eject();
    notice.textContent = `Could not open the recording: ${reason ||
      "the files don't match its game version"}.`;
    return;
  }

  remember(intake.version, intake.dataFiles());

  session = {
    transport: new Transport(engine.runtime),
    screen: new Screen(engine, $("canvas"), $("screen")),
    frame: 0,
    seeking: false,
  };

  $("seek").max = engine.runtime;
  session.screen.fit();
  session.frame = requestAnimationFrame(tick);
  $("canvas").focus();

  if (result === OpenResult.Damaged) {
    console.warn("The recording is damaged; playing what could be read.");
  }
}

function tick(stamp) {
  const { transport, screen } = session;
  const time = transport.advance(stamp);

  engine.seek(time);
  screen.paint();

  if (!session.seeking) {
    $("seek").value = time;
  }

  $("clock").textContent =
    `${formatTime(time)} / ${formatTime(transport.runtime)}`;
  $("play-button").toggleAttribute("data-paused", !transport.playing);
  $("play-button").setAttribute(
    "aria-label", transport.playing ? "Pause" : "Play");

  session.frame = requestAnimationFrame(tick);
}

function eject() {
  if (session !== null) {
    cancelAnimationFrame(session.frame);
    session.screen.dispose();
    session = null;
  }

  engine.close();
  app.dataset.state = "intake";
  $("stage").hidden = true;
  $("intake").hidden = false;
}

$("play-button").addEventListener("click", () => session.transport.toggle());
$("eject-button").addEventListener("click", eject);

$("speed").addEventListener("change", (event) => {
  session.transport.speed = Number(event.target.value);
});

$("seek").addEventListener("input", (event) => {
  session.seeking = true;
  session.transport.seek(Number(event.target.value));
});

$("seek").addEventListener("change", () => {
  session.seeking = false;
});

$("fullscreen-button").addEventListener("click", () => {
  if (document.fullscreenElement) {
    document.exitFullscreen();
  } else {
    $("stage").requestFullscreen();
  }
});

$("canvas").addEventListener("keydown", (event) => {
  const actions = {
    " ": () => session.transport.toggle(),
    ArrowLeft: () => session.transport.skip(-SKIP),
    ArrowRight: () => session.transport.skip(SKIP),
  };

  if (session !== null && actions[event.key]) {
    event.preventDefault();
    actions[event.key]();
  }
});

showIntake();
