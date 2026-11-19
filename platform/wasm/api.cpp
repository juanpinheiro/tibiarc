/*
 * Copyright 2026 "tibiarc contributors"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#include "assets/versions.hpp"
#include "recording/recordings.hpp"
#include "viewer.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <emscripten/emscripten.h>

/* The interface the web front end talks to. Everything is a plain C function
 * operating on one global player, as a page shows one recording at a time.
 *
 * Files are handed over as buffers allocated with `trc_alloc`; ownership
 * passes to the player, which keeps them for as long as it lives. */

using namespace trc;

namespace {

/* A file handed over by the page. */
struct Buffer {
    struct Release {
        void operator()(uint8_t *data) const {
            std::free(data);
        }
    };

    using Owned = std::unique_ptr<uint8_t, Release>;

    Owned Data;
    size_t Size = 0;

    DataReader Reader() const {
        return DataReader(Size, Data.get());
    }
};

struct Player {
    Buffer Pictures, Sprites, Types, Recording;
    std::unique_ptr<Version> Data;
    std::unique_ptr<Client::Viewer> Viewer;
    bool Damaged = false;
};

std::unique_ptr<Player> Current;
std::string LastError;

} // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE uint8_t *trc_alloc(size_t size) {
    return static_cast<uint8_t *>(std::malloc(size));
}

EMSCRIPTEN_KEEPALIVE void trc_free(uint8_t *data) {
    std::free(data);
}

EMSCRIPTEN_KEEPALIVE const char *trc_error() {
    return LastError.c_str();
}

/* Finds the game version a recording was made with, as major * 100 + minor,
 * or 0 if the file doesn't say. The buffer stays with the caller. */
EMSCRIPTEN_KEEPALIVE int trc_probe_version(const char *name,
                                           const uint8_t *data,
                                           size_t size) {
    try {
        const DataReader reader(size, data);
        const auto format = Recordings::GuessFormat(name, reader);
        VersionTriplet triplet;

        if (Recordings::QueryTibiaVersion(format, reader, triplet)) {
            return triplet.Major * 100 + triplet.Minor;
        }
    } catch (const ErrorBase &error) {
        LastError = error.Description();
    }

    return 0;
}

/* Opens a recording with the data files of its version. `version` is
 * major * 100 + minor, or 0 to take it from the recording. Takes ownership
 * of all four buffers. Returns 1 on success, 2 if the recording is damaged
 * and could only be read in part, and 0 on failure, see `trc_error`. */
EMSCRIPTEN_KEEPALIVE int trc_open(const char *name,
                                  int version,
                                  uint8_t *pictures,
                                  size_t picturesSize,
                                  uint8_t *sprites,
                                  size_t spritesSize,
                                  uint8_t *types,
                                  size_t typesSize,
                                  uint8_t *recording,
                                  size_t recordingSize,
                                  int width,
                                  int height) {
    auto player = std::make_unique<Player>();

    player->Pictures = Buffer{Buffer::Owned(pictures), picturesSize};
    player->Sprites = Buffer{Buffer::Owned(sprites), spritesSize};
    player->Types = Buffer{Buffer::Owned(types), typesSize};
    player->Recording = Buffer{Buffer::Owned(recording), recordingSize};

    Current.reset();
    LastError.clear();

    try {
        const DataReader reader = player->Recording.Reader();
        const auto format = Recordings::GuessFormat(name, reader);
        VersionTriplet triplet;

        if (version > 0) {
            triplet.Major = version / 100;
            triplet.Minor = version % 100;
        } else if (!Recordings::QueryTibiaVersion(format, reader, triplet)) {
            LastError = "The recording doesn't say which game version it "
                        "was made with.";
            return 0;
        }

        player->Data = std::make_unique<Version>(triplet,
                                                 player->Pictures.Reader(),
                                                 player->Sprites.Reader(),
                                                 player->Types.Reader());

        auto [parsed, partial] = Recordings::Read(format,
                                                  reader,
                                                  *player->Data,
                                                  Recordings::Recovery::Repair);

        player->Damaged = partial;
        player->Viewer = std::make_unique<Client::Viewer>(*player->Data,
                                                          std::move(parsed),
                                                          width,
                                                          height);
    } catch (const ErrorBase &error) {
        LastError = error.Description();
        return 0;
    }

    Current = std::move(player);

    return Current->Damaged ? 2 : 1;
}

EMSCRIPTEN_KEEPALIVE void trc_close() {
    Current.reset();
}

/* Length of the recording in milliseconds. */
EMSCRIPTEN_KEEPALIVE uint32_t trc_runtime() {
    return Current ? Current->Viewer->Playback.Runtime() : 0;
}

EMSCRIPTEN_KEEPALIVE uint32_t trc_now() {
    return Current ? Current->Viewer->Playback.Now() : 0;
}

EMSCRIPTEN_KEEPALIVE int trc_seek(uint32_t time) {
    if (!Current) {
        return 0;
    }

    try {
        Current->Viewer->Playback.Seek(time);
    } catch (const ErrorBase &error) {
        LastError = error.Description();
        return 0;
    }

    return 1;
}

EMSCRIPTEN_KEEPALIVE void trc_resize(int width, int height) {
    if (Current) {
        Current->Viewer->Resize(width, height);
    }
}

EMSCRIPTEN_KEEPALIVE void trc_set_console_height(int height) {
    if (Current) {
        Current->Viewer->SetConsoleHeight(height);
    }
}

/* Vertical extent of the bar between the game window and the console, so
 * the page can let it be dragged: top in the high 16 bits, height in the
 * low. */
EMSCRIPTEN_KEEPALIVE uint32_t trc_splitter() {
    if (!Current) {
        return 0;
    }

    const Rect &bar = Current->Viewer->Arrangement().Splitter;

    return (static_cast<uint32_t>(bar.Y) << 16) | bar.Height;
}

EMSCRIPTEN_KEEPALIVE int trc_console_height() {
    return Current ? Current->Viewer->Arrangement().Console.Height : 0;
}

/* Paints the window and returns its RGBA pixels, `trc_width` by
 * `trc_height`, valid until the next call into the player. */
EMSCRIPTEN_KEEPALIVE const uint8_t *trc_render() {
    if (!Current) {
        return nullptr;
    }

    try {
        return Current->Viewer->Render().Buffer;
    } catch (const ErrorBase &error) {
        LastError = error.Description();
        return nullptr;
    }
}

EMSCRIPTEN_KEEPALIVE int trc_width() {
    return Current ? Current->Viewer->Width() : 0;
}

EMSCRIPTEN_KEEPALIVE int trc_height() {
    return Current ? Current->Viewer->Height() : 0;
}

} /* extern "C" */
