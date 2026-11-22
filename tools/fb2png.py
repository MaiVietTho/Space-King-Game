#!/usr/bin/env python3
"""Screenshots and short videos of the kit's LCD, captured over the serial console.

Screenshot (shell command `lcd d`; close minicom first, the port must be free):
    python3 tools/fb2png.py --port /dev/ttyUSB0 -o resources/images/screens/scr_menu.png

Video as an animated GIF (shell command `lcd v`, one frame per LCD update), 10 s by default:
    python3 tools/fb2png.py --port /dev/ttyUSB0 --record -o resources/images/screens/round1.gif
    python3 tools/fb2png.py --port /dev/ttyUSB0 --record 5 -o ...      (5 s)

Convert a console log that contains one or more `lcd d` dumps:
    python3 tools/fb2png.py dump.txt -o resources/images/screens/scr_menu.png
    (several dumps -> scr_menu_1.png, scr_menu_2.png, ...)
"""
import argparse
import os
import re
import sys
import time

from PIL import Image

WIDTH, HEIGHT = 128, 64
FB_SIZE = WIDTH * HEIGHT // 8
START = "[DUMP] frame buffer lcd => start"
END = "[DUMP] frame buffer lcd => end"
STREAM_END = b"[STREAM] lcd => end"
PACKET_HEAD = b"\xA5\x5AF"      # see shell_lcd_stream_send_frame() in shell.cpp

STYLES = {
    "oled": ((12, 16, 28), (220, 235, 255)),   # same look as tools/gen_bitmaps.py
    "bw": ((0, 0, 0), (255, 255, 255)),
}


def parse_dumps(text):
    """Return every framebuffer found between the `lcd d` start / end markers."""
    dumps = []
    for block in re.findall(re.escape(START) + r"(.*?)" + re.escape(END), text, re.S):
        block = re.sub(r"width=\d+ height=\d+ bytes=\d+", "", block)
        data = [int(v, 16) for v in re.findall(r"0x([0-9A-Fa-f]{2})", block)]
        if len(data) != FB_SIZE:
            print(f"skip dump: {len(data)} bytes instead of {FB_SIZE}", file=sys.stderr)
            continue
        dumps.append(data)
    return dumps


class PacketParser:
    """Incremental parser for the binary RLE frames of `lcd v`."""

    def __init__(self):
        self.buf = bytearray()
        self.bad = 0

    def feed(self, chunk):
        self.buf += chunk
        frames = []
        while True:
            i = self.buf.find(PACKET_HEAD)
            if i < 0:
                del self.buf[:max(0, len(self.buf) - 2)]
                return frames
            if len(self.buf) < i + 5:
                return frames
            length = self.buf[i + 3] | (self.buf[i + 4] << 8)
            end = i + 5 + length + 1
            if len(self.buf) < end:
                return frames
            payload, check = self.buf[i + 5:end - 1], self.buf[end - 1]
            frame = self.decode(payload, check)
            if frame is None:
                self.bad += 1
                del self.buf[:i + 1]        # resync on the next header
                continue
            frames.append(frame)
            del self.buf[:end]

    @staticmethod
    def decode(payload, check):
        if len(payload) % 2:
            return None
        x = 0
        data = bytearray()
        for k in range(0, len(payload), 2):
            count, value = payload[k], payload[k + 1]
            x ^= count ^ value
            data += bytes([value]) * count
        if x != check or len(data) != FB_SIZE:
            return None
        return list(data)


def to_image(data, scale, style):
    """SSD1306/SH1106 page layout: byte (y / 8) * 128 + x, bit y % 8."""
    bg, fg = STYLES[style]
    img = Image.new("RGB", (WIDTH, HEIGHT), bg)
    for y in range(HEIGHT):
        for x in range(WIDTH):
            if data[(y // 8) * WIDTH + x] & (1 << (y % 8)):
                img.putpixel((x, y), fg)
    return img.resize((WIDTH * scale, HEIGHT * scale), Image.NEAREST)


def open_port(port):
    import termios

    fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    attrs = termios.tcgetattr(fd)
    attrs[0] = 0                                        # iflag
    attrs[1] = 0                                        # oflag
    attrs[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
    attrs[3] = 0                                        # lflag: raw
    attrs[4] = attrs[5] = termios.B115200
    termios.tcsetattr(fd, termios.TCSANOW, attrs)
    termios.tcflush(fd, termios.TCIOFLUSH)
    return fd


def read_chunk(fd):
    try:
        return os.read(fd, 4096)
    except BlockingIOError:
        time.sleep(0.01)
        return b""


def capture(port, timeout):
    """Send `lcd d` and return the received text."""
    fd = open_port(port)
    try:
        os.write(fd, b"lcd d\r")
        text, deadline = "", time.time() + timeout
        while time.time() < deadline and END not in text:
            text += read_chunk(fd).decode("ascii", "replace")
        return text
    finally:
        os.close(fd)


def record(port, seconds):
    """Send `lcd v <seconds>` and return [(arrival time, framebuffer)]."""
    fd = open_port(port)
    parser, frames, tail = PacketParser(), [], b""
    try:
        os.write(fd, f"lcd v {seconds}\r".encode())
        deadline = time.time() + seconds + 2
        while time.time() < deadline:
            chunk = read_chunk(fd)
            now = time.time()
            frames += [(now, f) for f in parser.feed(chunk)]
            tail = (tail + chunk)[-64:]
            if STREAM_END in tail:
                break
        os.write(fd, b"lcd x\r")        # make sure the console is unmuted
    finally:
        os.close(fd)
    if parser.bad:
        print(f"dropped {parser.bad} corrupted frame(s)", file=sys.stderr)
    return frames, time.time()


def save_gif(frames, end_time, path, scale, style):
    images, durations = [], []
    for k, (t, data) in enumerate(frames):
        nxt = frames[k + 1][0] if k + 1 < len(frames) else min(end_time, t + 1)
        images.append(to_image(data, scale, style))
        durations.append(max(20, int(round((nxt - t) * 100)) * 10))   # GIF uses 10 ms units
    images[0].save(path, save_all=True, append_images=images[1:], loop=0,
                   duration=durations, disposal=2, optimize=False)
    return sum(durations) / 1000


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("log", nargs="?", help="console log containing `lcd d` dumps (omit when using --port)")
    ap.add_argument("--port", help="serial port of the kit, e.g. /dev/ttyUSB0")
    ap.add_argument("--record", type=int, nargs="?", const=10, metavar="SECONDS",
                    help="record a video instead of a screenshot (1-60 s, default 10)")
    ap.add_argument("-o", "--output", help="output file (default: screen.png, or screen.gif with --record)")
    ap.add_argument("--scale", type=int, default=4, help="pixel scale (default: 4 -> 512x256)")
    ap.add_argument("--style", choices=STYLES, default="oled", help="colors (default: oled)")
    ap.add_argument("--timeout", type=float, default=5, help="screenshot read timeout in s (default: 5)")
    args = ap.parse_args()
    output = args.output or ("screen.gif" if args.record else "screen.png")
    os.makedirs(os.path.dirname(output) or ".", exist_ok=True)

    try:
        if args.record:
            if not args.port:
                ap.error("--record needs --port")
            if not 1 <= args.record <= 60:
                ap.error("--record must be 1..60 s")
            frames, end_time = record(args.port, args.record)
            if not frames:
                sys.exit("no frame received (is the firmware built with the `lcd v` command?)")
            length = save_gif(frames, end_time, output, args.scale, args.style)
            print(f"saved {output}: {len(frames)} frames, {length:.1f} s")
            return
        if args.port:
            text = capture(args.port, args.timeout)
        elif args.log:
            text = open(args.log, encoding="utf-8", errors="replace").read()
        else:
            ap.error("give a log file or --port")
    except OSError as e:
        sys.exit(f"cannot open {args.port}: {e.strerror}\n"
                 "- Input/output error: the USB-UART chip is not responding; replug the board "
                 "(directly, not through a hub) or try another cable, then check `journalctl -k -n 5`\n"
                 "- Permission denied / busy: join the `dialout` group, close minicom")

    dumps = parse_dumps(text)
    if not dumps:
        sys.exit("no complete dump found (is the firmware built with the `lcd d` command?)")

    base, ext = os.path.splitext(output)
    for i, data in enumerate(dumps, 1):
        path = output if len(dumps) == 1 else f"{base}_{i}{ext or '.png'}"
        to_image(data, args.scale, args.style).save(path)
        print("saved", path)


if __name__ == "__main__":
    main()
