/* The playback clock: turns wall time into recording time. */

export class Transport {
  constructor(runtime) {
    this.runtime = runtime;
    this.time = 0;
    this.speed = 1;
    this.playing = true;
    this.lastStamp = null;
  }

  /* Advances to the given animation-frame timestamp and returns the
   * recording time to show. */
  advance(stamp) {
    if (this.playing && this.lastStamp !== null) {
      this.time = Math.min(
        this.runtime, this.time + (stamp - this.lastStamp) * this.speed);

      if (this.time >= this.runtime) {
        this.playing = false;
      }
    }

    this.lastStamp = stamp;

    return Math.floor(this.time);
  }

  toggle() {
    /* Pressing play at the end starts over. */
    if (!this.playing && this.time >= this.runtime) {
      this.time = 0;
    }

    this.playing = !this.playing;
  }

  seek(time) {
    this.time = Math.max(0, Math.min(this.runtime, time));
  }

  skip(delta) {
    this.seek(this.time + delta);
  }
}

export function formatTime(milliseconds) {
  const seconds = Math.floor(milliseconds / 1000);
  const hours = Math.floor(seconds / 3600);
  const minutes = Math.floor(seconds / 60) % 60;
  const rest = String(seconds % 60).padStart(2, "0");

  return hours > 0
    ? `${hours}:${String(minutes).padStart(2, "0")}:${rest}`
    : `${minutes}:${rest}`;
}
