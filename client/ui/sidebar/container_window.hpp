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

#ifndef __TRC_CLIENT_UI_SIDEBAR_CONTAINER_WINDOW_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_CONTAINER_WINDOW_HPP__

#include "game/container.hpp"
#include "ui/sidebar/content_window.hpp"

namespace trc {
namespace Client {

/* The window of an open container: a grid with a cell for every object the
 * container has room for, four to a row. */
class ContainerWindow : public ContentWindow {
public:
    /* `initialItems` is how many objects the container held when it was
     * opened, which decides how many rows the window starts out with. */
    ContainerWindow(const Container &container, int initialItems)
        : Contents(container), InitialItems(initialItems) {
    }

    int MinimumHeight() const override;
    int InitialHeight() const override;
    int ScrollStep() const override;

protected:
    std::string_view Title() const override;
    void DrawIcon(Canvas &canvas,
                  Widgets &widgets,
                  int x,
                  int y,
                  uint32_t now) const override;
    bool HasUpButton() const override;
    int ContentHeight() const override;
    void DrawList(Canvas &canvas,
                  Widgets &widgets,
                  const Rect &area,
                  uint32_t now) const override;

private:
    const Container &Contents;
    int InitialItems;
};

} // namespace Client
} // namespace trc

#endif
