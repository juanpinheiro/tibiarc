/*
 * Copyright 2026 "Juan Pinheiro"
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

#ifndef __TRC_CLIENT_UI_WIDGETS_HPP__
#define __TRC_CLIENT_UI_WIDGETS_HPP__

#include "assets/versions.hpp"
#include "game/creature.hpp"
#include "game/object.hpp"
#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "scene/outfits.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace trc {
namespace Client {

/* How a piece of text is drawn: one of the four fonts by its number, tinted
 * and anchored at its left edge, centre or right edge. */
struct TextStyle {
    int Font;
    Pixel Color;
    Align Alignment;
};

/* The two faces of a button. Nothing in the interface reacts to the pointer
 * merely hovering over it. */
struct ButtonSkins {
    int Up;
    int Down;

    /* The buttons that carry a caption. */
    static const ButtonSkins Regular;
    static const ButtonSkins Narrow;
};

/* The building blocks the interface is made of. Everything is drawn within
 * the clip rectangle of the canvas. */
class Widgets {
public:
    /* Width of a vertical scrollbar, and the least height it works at. */
    static constexpr int ScrollbarWidth = 12;
    static constexpr int ScrollbarMinimumHeight = 38;

    Widgets(const Version &version, const Fonts &fonts, const Skin &skin);

    const Fonts &Text;
    const Skin &Pieces;

    /* Draws text with its top at `y`, anchored at `x` as the style says. */
    void Write(Canvas &canvas,
               int x,
               int y,
               const TextStyle &style,
               std::string_view text) const;

    /* Draws a line of text inside `area`: placed horizontally by the
     * alignment of its style, centred vertically, and cut off at the
     * edges. */
    void Label(Canvas &canvas,
               const Rect &area,
               const TextStyle &style,
               std::string_view text) const;

    /* Cuts a text that is wider than `maxWidth` short, ending it in three
     * dots that take the place of the last characters that fit. */
    std::string Abbreviate(int font,
                           std::string_view text,
                           int maxWidth) const;

    void Button(Canvas &canvas,
                const ButtonSkins &skins,
                int x,
                int y,
                bool down = false) const;

    /* A button with a caption, which moves along when the button is down. */
    void TextButton(Canvas &canvas,
                    const ButtonSkins &skins,
                    int x,
                    int y,
                    std::string_view caption,
                    bool down = false) const;

    /* A vertical scrollbar resting at the top of `total` rows of pixels of
     * which `page` are in view. There's no thumb when everything is. */
    void Scrollbar(Canvas &canvas,
                   int x,
                   int y,
                   int height,
                   int total,
                   int page) const;

    /* A framed bar of which `percent` hundredths are filled. What isn't is
     * left as it was. */
    void Gauge(Canvas &canvas,
               const Rect &frame,
               int percent,
               const Pixel &color) const;

    /* Draws an object scaled into a square of `size` pixels with its
     * bottom-right corner in that of the square, together with its count if
     * it is a pile. */
    void ObjectIcon(Canvas &canvas,
                    int x,
                    int y,
                    int size,
                    const Object &object,
                    uint32_t now);

    /* Draws a creature facing south, standing still, the same way. */
    void OutfitIcon(Canvas &canvas,
                    int x,
                    int y,
                    int size,
                    const Appearance &outfit,
                    uint32_t now);

private:
    /* Draws the part of a picture that is `column` parts left of and `row`
     * parts above its bottom-right one, each part `scale` pixels a side. */
    void DrawPart(Canvas &canvas,
                  const Sprite &sprite,
                  int x,
                  int y,
                  int size,
                  int scale,
                  int column,
                  int row);

    const Version &Data;
    OutfitColorizer Colorizer;
    /* Where a sprite is unpacked before it is scaled. */
    Canvas Scratch;
};

} // namespace Client
} // namespace trc

#endif
