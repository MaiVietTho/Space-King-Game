#!/usr/bin/env python3
"""Render the game's monochrome bitmaps (screens_bitmap.cpp) to PNG / GIF files for the docs.

Screen shots and gameplay videos come from the real kit instead: see tools/fb2png.py.

Usage (from the repo root):
    python3 tools/gen_bitmaps.py
"""
import os
import re

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "ak-base-kit-stm32l151/application/sources/app/screens/screens_bitmap.cpp")
OUT_BITMAP = os.path.join(ROOT, "resources/images/bitmap")

# OLED look: dark panel, light pixels
BG = (12, 16, 28)
FG = (220, 235, 255)


def load_bitmaps():
    text = open(SRC, encoding="utf-8").read()
    pattern = re.compile(
        r"//\s*'(\w+)',\s*(\d+)x(\d+)px\s*\n\s*const unsigned char PROGMEM\s+(\w+)\s*\[\]\s*=\s*\{(.*?)\};",
        re.S,
    )
    bitmaps = {}
    for _, w, h, name, body in pattern.findall(text):
        data = [int(v, 16) for v in re.findall(r"0x[0-9a-fA-F]+", body)]
        bitmaps[name] = (int(w), int(h), data)
    return bitmaps


def pixels(w, h, data):
    """Adafruit GFX drawBitmap layout: row-major, MSB first, rows padded to whole bytes."""
    byte_w = (w + 7) // 8
    for y in range(h):
        for x in range(w):
            idx = y * byte_w + x // 8
            if idx < len(data) and data[idx] & (0x80 >> (x % 8)):
                yield x, y


def render(w, h, data, scale, pad=1):
    img = Image.new("RGB", ((w + 2 * pad) * scale, (h + 2 * pad) * scale), BG)
    for x, y in pixels(w, h, data):
        x0, y0 = (x + pad) * scale, (y + pad) * scale
        img.paste(FG, (x0, y0, x0 + scale, y0 + scale))
    return img


def blit(canvas, bitmap, x, y):
    w, h, data = bitmap
    for px, py in pixels(w, h, data):
        if 0 <= x + px < canvas.width and 0 <= y + py < canvas.height:
            canvas.putpixel((x + px, y + py), FG)


def animation(bitmaps, frames, path, scale=6, pad=1):
    """Write an animated GIF. frames: list of (bitmap name or None, dx, dy, duration ms)."""
    w = max(bitmaps[n][0] + dx for n, dx, _, _ in frames if n)
    h = max(bitmaps[n][1] + dy for n, _, dy, _ in frames if n)
    images = []
    for name, dx, dy, _ in frames:
        canvas = Image.new("RGB", (w + 2 * pad, h + 2 * pad), BG)
        if name:
            blit(canvas, bitmaps[name], dx + pad, dy + pad)
        images.append(canvas.resize((canvas.width * scale, canvas.height * scale), Image.NEAREST))
    images[0].save(path, save_all=True, append_images=images[1:], loop=0,
                   duration=[d for *_, d in frames], disposal=2, optimize=False)


def main():
    os.makedirs(OUT_BITMAP, exist_ok=True)
    bitmaps = load_bitmaps()

    objects = [
        "bitmap_spacecraft", "bitmap_arrow",
        "bitmap_enemy_I", "bitmap_enemy_II", "bitmap_enemy_III", "bitmap_arrow_enemy",
        "bitmap_meteoroid", "bitmap_gift_heart", "bitmap_gift_arrow_power",
        "bitmap_Boss_I", "bitmap_Boss_II", "bitmap_Boss_III",
        "bitmap_energy_ball", "bitmap_Laser",
        "bitmap_bang_I", "bitmap_bang_II", "bitmap_bang_III",
    ]
    for name in objects:
        w, h, data = bitmaps[name]
        scale = 3 if w > 60 else 6
        render(w, h, data, scale).save(os.path.join(OUT_BITMAP, f"{name}.png"))

    # Animations with the frame order and timing of the game (1 tick = 100 ms)
    # Enemy: action_image 1..6 -> I, I, II, II, III, III (sc_game_enemy.cpp, scr_spacecraft_game.cpp)
    animation(bitmaps, [("bitmap_enemy_I", 0, 0, 200), ("bitmap_enemy_II", 0, 0, 200),
                        ("bitmap_enemy_III", 0, 0, 200)],
              os.path.join(OUT_BITMAP, "anim_enemy.gif"))
    # Boss: action_image 1..4 -> I, I, II, II (sc_game_boss.cpp)
    animation(bitmaps, [("bitmap_Boss_I", 0, 0, 200), ("bitmap_Boss_II", 0, 0, 200)],
              os.path.join(OUT_BITMAP, "anim_boss.gif"))
    # Bang: 1 tick per frame, frame III drawn at (+2, -1), then hidden (pause added for the loop)
    animation(bitmaps, [("bitmap_bang_I", 0, 1, 100), ("bitmap_bang_II", 0, 1, 100),
                        ("bitmap_bang_III", 2, 0, 100), (None, 0, 0, 500)],
              os.path.join(OUT_BITMAP, "anim_bang.gif"))

    print("bitmaps written to", OUT_BITMAP)


if __name__ == "__main__":
    main()
