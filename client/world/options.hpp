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

#ifndef __TRC_CLIENT_WORLD_OPTIONS_HPP__
#define __TRC_CLIENT_WORLD_OPTIONS_HPP__

namespace trc {
namespace Client {

/* The preferences of the game client that affect what is shown, with the
 * client's own defaults. */
struct Options {
    enum class CreatureInfo { None = 0, Bars = 1, NamesAndBars = 2 };

    CreatureInfo ShowCreatureInfo = CreatureInfo::NamesAndBars;
    bool ShowTextualEffects = true;
    bool ShowPrivateMessages = true;

    /* Which server messages are copied to the console. */
    bool ShowEventMessages = true;
    bool ShowStatusMessages = false;
    bool ShowInfoMessages = false;

    /* Draws the map at its native size, cropped, instead of scaling it to
     * the game window. */
    bool NoStretching = false;
};

} // namespace Client
} // namespace trc

#endif
