# Rendering

How `Viewer::Render` puts a frame together. Everything is drawn in software
into a 32-bit canvas (`core/graphics/canvas.hpp`).

## The window

```
┌──────────────────────────────┬────────┐
│ game window                  │ side-  │
│  ┌────────────────────────┐  │ bar    │
│  │ view (map picture)     │  │ 176 px │
│  └────────────────────────┘  │        │
├──────── splitter ────────────┤        │
│ console                      │        │
└──────────────────────────────┴────────┘
```

`client/ui/layout.cpp` computes the rectangles. The sidebar has a fixed
width. The view keeps the 15:11 aspect of the map and is as large as the
room above the console allows; the console gets what remains, at least its
minimum height. The window is never smaller than 550×450.

Interface pieces come from one picture in `Tibia.pic` and are **tiled, never
stretched** (`client/resources/skin.cpp`).

## The map

`client/scene/mapview.cpp` composes the visible part of the map at its
native size, **480×352 pixels** (15×11 fields of 32 pixels), from an 18×14
window of fields around the player:

1. **Visible floors.** Floors above the player are hidden where something
   would cover the player's view; a field with a full ground hides what is
   below it.
2. **Fields**, floor by floor, back to front. Within a field: ground and
   borders in stack order, other objects from oldest to newest, then
   creatures, effects, and finally objects that are always on top.
   Elevated objects raise what stands on them.
3. **Creatures** are drawn with the field they are walking out of, sorted
   by their pixel offset, so tall sprites overlap correctly. Outfit colours
   are applied through the sprite's mask (`scene/outfits.cpp`).
4. **Light.** Each field has a light colour: the ambient light, raised by
   light sources with a falloff over their radius. The map is multiplied by
   that light interpolated smoothly across fields (`scene/light.cpp`).

## Scaling and overlays

The 480×352 picture is scaled to the view with **bilinear filtering**. Only
then is everything that carries text drawn, in window pixels, at its own
size (`client/scene/overlay.cpp`):

- creature names and health bars, skulls and party marks;
- speech, laid out so that boxes don't overlap;
- the status line and centred messages;
- numbers rising from creatures.

Their positions follow the map; their size never changes. This is what keeps
text crisp at any window size — scaling a picture that already contains
text is what makes it blurry or blocky.

## Time

The client animates on a 100 ms timer, and so does everything here
(`client/scene/animation.hpp`); the tick is derived from the playback time,
so any moment can be painted on its own.

| What | Pace |
| --- | --- |
| Animated objects | one frame per 5 ticks |
| Idle-animated outfits | one frame per 3 ticks |
| Effects | one frame per tick |
| Rising numbers | 2 pixels per tick, for 10 ticks |
| Ambient light changes | one step per second |
| Walking | offset shrinks linearly over the step's duration |

A step lasts `ground speed × 1000 / creature speed` milliseconds, rounded up
to the server's beat; diagonal steps take three times as long to settle.
The walking frame follows the distance left, one frame per 8 pixels.

## Checking the picture

`tibiarc-render` writes what the client paints to PNG; see
[Command-line tools](tools.md). When changing anything here, compare frames
before and after — the goal is the original client's picture, pixel for
pixel, not a nicer one.
