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

#include "ui/console/text_view.hpp"

#include "ui/console/scrollbar.hpp"

#include <algorithm>
#include <string_view>
#include <vector>

namespace trc {
namespace Client {

/* Skins of the frame around the text. */
enum FrameSkin : int {
    FrameTopLeft = 169,
    FrameTopRight = 170,
    FrameBottomLeft = 171,
    FrameBottomRight = 172,
    FrameLeft = 173,
    FrameRight = 174,
    FrameTop = 175,
    FrameBottom = 176,
};

static constexpr int ScrollbarWidth = 12;

/* Room between the frame's outer edge and the text: on the right it holds
 * the scrollbar as well. */
static constexpr int TextMargin = 5;
static constexpr int TextMarginRight = 17;

/* How far the lines after the first are pushed in when a message doesn't
 * fit on a single one. */
static constexpr int ContinuationIndent = 10;

struct TextLine {
    std::string_view Text;
    int Indent;
    Pixel Color;
};

static bool IsSpace(char character) {
    return character == ' ' || (character >= '\t' && character <= '\r');
}

/* Breaks a message into lines no wider than `width`. Lines end at the last
 * whitespace that fits, which is dropped, or in the middle of a word when
 * there is none; a line feed always ends the line. */
static void Wrap(const Font &font,
                 const ConsoleMessage &message,
                 int width,
                 std::vector<TextLine> &lines) {
    const Pixel color = TalkColor(message.Kind);
    const int continuation =
            IsServerMessage(message.Kind) ? 0 : ContinuationIndent;

    std::string_view rest = message.Text;
    int indent = 0;

    while (!rest.empty()) {
        /* Always make headway, even when there's no room at all. */
        const size_t fit =
                std::max<size_t>(font.TextFit(rest, width - indent), 1);
        const std::string_view fitting = rest.substr(0, fit);
        size_t length = fit, next = fit;

        if (const size_t feed = fitting.find('\n'); feed != fitting.npos) {
            length = feed;
            next = feed + 1;
        } else if (fit < rest.size()) {
            const auto space =
                    std::find_if(fitting.rbegin(), fitting.rend(), IsSpace);

            if (space != fitting.rend()) {
                length = static_cast<size_t>(fitting.rend() - space) - 1;
                next = length + 1;
            }
        }

        /* Empty lines aren't kept. */
        if (length > 0) {
            lines.push_back(TextLine{rest.substr(0, length), indent, color});
        }

        rest.remove_prefix(next);
        indent = continuation;
    }
}

static void DrawFrame(Canvas &canvas, const Skin &skin, const Rect &area) {
    const int edge = Skin::Width(FrameLeft);
    const int right = area.Right() - edge, bottom = area.Bottom() - edge;

    skin.Draw(canvas, FrameTopLeft, area.X, area.Y);
    skin.Tile(canvas,
              FrameTop,
              Rect{area.X + edge, area.Y, area.Width - 2 * edge, edge});
    skin.Draw(canvas, FrameTopRight, right, area.Y);
    skin.Tile(canvas,
              FrameLeft,
              Rect{area.X, area.Y + edge, edge, area.Height - 2 * edge});
    skin.Tile(canvas,
              FrameRight,
              Rect{right, area.Y + edge, edge, area.Height - 2 * edge});
    skin.Draw(canvas, FrameBottomLeft, area.X, bottom);
    skin.Tile(canvas,
              FrameBottom,
              Rect{area.X + edge, bottom, area.Width - 2 * edge, edge});
    skin.Draw(canvas, FrameBottomRight, right, bottom);
}

/* The number of lines that fit in `height`: the bottom one only needs the
 * height of its characters, those above it a full line each. */
static int LinesFitting(const Font &font, int height) {
    const int top = height - font.LineHeight();

    return (top < 0) ? 0 : top / font.LinePitch() + 1;
}

void DrawConsoleText(Canvas &canvas,
                     const Fonts &fonts,
                     const Skin &skin,
                     const Rect &area,
                     const ConsoleChannel &channel,
                     int &linesBack) {
    const Font &font = fonts.Interface;
    const int edge = Skin::Width(FrameLeft);
    const Rect text{area.X + TextMargin,
                    area.Y + TextMargin,
                    area.Width - TextMargin - TextMarginRight,
                    area.Height - 2 * TextMargin};
    std::vector<TextLine> lines;

    for (const auto &message : channel.Messages) {
        Wrap(font, message, text.Width, lines);
    }

    /* The scrollbar counts in lines and points at the one along the bottom,
     * which is the newest unless the text was scrolled back. A full view
     * is the furthest back it goes. */
    const int newest = static_cast<int>(lines.size()) - 1;
    const int fitting = LinesFitting(font, text.Height);
    const int page = std::min(newest, std::max(fitting - 1, 0));

    linesBack = std::clamp(linesBack, 0, newest - page);

    const int last = newest - linesBack;
    const int first = std::max(last - fitting + 1, 0);

    DrawFrame(canvas, skin, area);
    DrawScrollbar(canvas,
                  skin,
                  Rect{area.Right() - edge - ScrollbarWidth,
                       area.Y + edge,
                       ScrollbarWidth,
                       area.Height - 2 * edge},
                  ScrollRange{page, newest, last, page});

    /* Lines are placed from the bottom up, and only whole ones are shown. */
    int y = text.Bottom() - font.LineHeight();

    for (int index = last; index >= first; index--) {
        const TextLine &line = lines[index];

        font.Draw(canvas, text.X + line.Indent, y, line.Color, line.Text);
        y -= font.LinePitch();
    }
}

} // namespace Client
} // namespace trc
