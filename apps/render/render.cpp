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
#include "shared/cli.hpp"
#include "shared/memoryfile.hpp"
#include "shared/png.hpp"
#include "viewer.hpp"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

using namespace trc;

/* Renders still frames of a recording to PNG files, for inspecting what the
 * client draws without a browser. */
int main(int argc, char **argv) {
    int width = 800, height = 600;
    uint32_t start = 0, step = 1000, count = 1;
    bool discard = false;
    VersionTriplet desired;

    auto paths = CLI::Process(
            argc,
            argv,
            "tibiarc-render -- dumps frames of a Tibia recording as PNG",
            "tibiarc-render 0.1",
            {"data_folder", "input_path", "output_prefix"},
            {{"size",
              {"window size in pixels",
               {"width", "height"},
               [&](const CLI::Range &args) {
                   width = std::stoi(args[0]);
                   height = std::stoi(args[1]);
               }}},
             {"start",
              {"time of the first frame, in milliseconds",
               {"ms"},
               [&](const CLI::Range &args) { start = std::stoul(args[0]); }}},
             {"step",
              {"time between frames, in milliseconds",
               {"ms"},
               [&](const CLI::Range &args) { step = std::stoul(args[0]); }}},
             {"count",
              {"number of frames to write",
               {"frames"},
               [&](const CLI::Range &args) { count = std::stoul(args[0]); }}},
             {"discard",
              {"paint the frames without writing them; for testing",
               {},
               [&]([[maybe_unused]] const CLI::Range &args) {
                   discard = true;
               }}},
             {"input-version",
              {"the Tibia version of the recording, in case it can't be "
               "detected",
               {"tibia_version"},
               [&](const CLI::Range &args) {
                   if (sscanf(args[0].c_str(),
                              "%u.%u.%u",
                              &desired.Major,
                              &desired.Minor,
                              &desired.Preview) < 2) {
                       throw "input-version must be in the format 'X.Y'";
                   }
               }}}});

    try {
        const std::filesystem::path dataFolder = paths[0];
        const std::filesystem::path inputPath = paths[1];

        const MemoryFile file(inputPath);
        const DataReader reader = file.Reader();
        const auto format = Recordings::GuessFormat(inputPath, reader);

        if (desired == VersionTriplet() &&
            !Recordings::QueryTibiaVersion(format, reader, desired)) {
            std::cerr << "error: cannot tell the version of the recording, "
                         "pass --input-version"
                      << std::endl;
            return 1;
        }

        const MemoryFile pictures(dataFolder / "Tibia.pic");
        const MemoryFile sprites(dataFolder / "Tibia.spr");
        const MemoryFile types(dataFolder / "Tibia.dat");
        const Version version(desired,
                              pictures.Reader(),
                              sprites.Reader(),
                              types.Reader());

        auto [recording, partial] = Recordings::Read(format,
                                                     reader,
                                                     version,
                                                     Recordings::Recovery::Repair);

        if (partial) {
            std::cerr << "warning: the recording is damaged and was only "
                         "partially read"
                      << std::endl;
        }

        Client::Viewer viewer(version, std::move(recording), width, height);

        std::cerr << "runtime: " << viewer.Playback.Runtime() << " ms"
                  << std::endl;

        for (uint32_t index = 0; index < count; index++) {
            char suffix[32];

            viewer.Playback.Seek(start + index * step);

            const Canvas &picture = viewer.Render();

            if (!discard) {
                snprintf(suffix, sizeof(suffix), "%04u.png", index);
                PNG::Write(picture, paths[2] + suffix);
            }
        }
    } catch (const ErrorBase &error) {
        std::cerr << "error: " << error.Description() << std::endl;
        return 1;
    }

    return 0;
}
