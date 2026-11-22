# Công cụ sinh tài liệu (`tools/`)

Thư mục này chứa các script Python sinh **sơ đồ sequence** và **hình ảnh** dùng trong `README.md` và `docs/`. Script không đụng tới firmware; chúng chỉ ghi file vào `resources/`.

## I. Các file

| File | Chạy trực tiếp? | Dùng để làm gì | Ghi ra |
|---|---|---|---|
| `gen_diagrams.py` | Có | Chứa **nội dung** của 16 sơ đồ sequence (mỗi sơ đồ một hàm) và ghi chúng ra file | `resources/diagrams/*.drawio.svg` |
| `fb2png.py` | Có | Chụp màn hình (`lcd d`) hoặc quay video (`lcd v`) màn hình thật của board, lưu thành PNG / GIF | file bạn chọn, thường là `resources/images/screens/` |
| `seqdraw.py` | Không | **Thư viện vẽ** mà `gen_diagrams.py` dùng: tính bố cục, xuất SVG kèm mô hình draw.io | (không tự ghi) |
| `gen_bitmaps.py` | Có | Đọc bitmap trong `screens_bitmap.cpp`, render thành PNG và GIF động cho bảng Objects | `resources/images/bitmap/*.png`, `*.gif` |

## II. Yêu cầu

- **Python 3:** đủ cho `gen_diagrams.py`.
- **Pillow:** cần thêm cho `gen_bitmaps.py`. Kiểm tra bằng `python3 -c "import PIL"`; nếu thiếu thì cài:
  ```sh
  sudo apt install python3-pil
  ```

## III. Lệnh

Luôn chạy từ **thư mục gốc của repo**:

```sh
cd ~/Documents/Space-King-Game

python3 tools/gen_diagrams.py    # -> "16 diagrams written to .../resources/diagrams"
python3 tools/gen_bitmaps.py     # -> "bitmaps written to .../resources/images/bitmap"

# Cần board cắm USB + firmware có lệnh lcd d / lcd v (xem mục VI)
python3 tools/fb2png.py --port /dev/ttyUSB0 -o resources/images/screens/scr_menu_game.png   # chụp 1 hình
python3 tools/fb2png.py --port /dev/ttyUSB0 --record -o resources/images/screens/round1.gif # quay 10 s
```

`gen_diagrams.py` và `gen_bitmaps.py` tự tìm đường dẫn theo vị trí của chính nó. Riêng `fb2png.py` lưu file theo đường dẫn `-o` tính từ thư mục hiện tại, nên **phải chạy ở gốc repo** thì ảnh mới vào đúng `resources/images/screens/`.

## IV. Chạy gì khi game có thay đổi

| Bạn thay đổi gì trong game | Việc cần làm |
|---|---|
| Vẽ lại / thêm bitmap trong `screens_bitmap.cpp` | Chạy `gen_bitmaps.py` (bitmap mới phải thêm vào danh sách, xem V.2) |
| Đổi luồng game: thêm/bớt signal, đổi timer, đổi thứ tự xử lý | Sửa hàm sơ đồ tương ứng trong `gen_diagrams.py` → chạy `gen_diagrams.py` → cập nhật đoạn mô tả trong `docs/01` hoặc `docs/02` |
| Thêm object mới | Thêm hàm sơ đồ mới + đăng ký vào `DIAGRAMS` (xem V.1) → chạy `gen_diagrams.py` → thêm mục mới trong `docs/01` (mô tả + `<img>` trỏ tới file mới + dòng trong bảng tổng quan và bảng tham chiếu) → nếu object có bitmap thì chạy `gen_bitmaps.py` và thêm dòng vào bảng *Objects* trong `README.md` |
| Đổi giao diện một màn hình, hoặc gameplay của một round | Chụp / quay lại bằng `fb2png.py` với đúng tên file đang dùng trong README (xem VI.4) |
| Đổi luật chơi (máu, số round, điều khiển) | Không cần script; sửa chữ trong `README.md` (*Rounds*, *How to Play*) |
| Đổi interval timer trong `app.h` | Sửa con số trong sơ đồ liên quan (`gen_diagrams.py`) và trong bảng *Timer* của `docs/02` |
| Chỉ sửa lỗi bên trong một hàm, luồng signal không đổi | Không cần làm gì |

### Thứ tự các bước khi cập nhật

1. Sửa code game, build và test trên board.
2. Sửa `tools/gen_diagrams.py` (và danh sách bitmap trong `gen_bitmaps.py` nếu có bitmap mới).
3. Chạy script:
   ```sh
   python3 tools/gen_diagrams.py
   python3 tools/gen_bitmaps.py
   ```
4. Mở vài file vừa sinh để kiểm tra (VSCode preview, trình duyệt, hoặc draw.io).
5. Cập nhật chữ trong `README.md`, `docs/01`, `docs/02` cho khớp.
6. Kiểm tra thay đổi rồi commit **cùng lúc** code + script + file sinh ra:
   ```sh
   git status
   git add tools resources docs README.md
   git commit -m "docs: update diagrams for <thay đổi>"
   ```

## V. Cách sửa nội dung

### 1. Sơ đồ (`gen_diagrams.py`)

Mỗi sơ đồ là một hàm trả về đối tượng `Seq`. Ví dụ:

```python
def ob_boss():
    d = P(Seq("Boss - round 5"), GAME, ("bs", "Boss", "object"), ("gf", "Gift", "object"))
    d.msg("game", "bs", "SC_GAME_BOSS_SETUP", "async")
    d.self_("bs", "x = 80, y = 30, hp = level * 50")
    d.frag("loop", "every tick")
    d.msg("game", "bs", "BOSS_MOVE", "async")
    d.end()
    return d
```

**Participant** (các cột): `P(Seq("Tiêu đề"), p1, p2, ...)`. Mỗi participant là tuple `(khóa, "Tên hiển thị", loại)`. Thứ tự khai báo là thứ tự cột từ trái sang phải. Có sẵn các hằng `BTN`, `TMR`, `DISP`, `GAME`, `EEP`.

| Loại | Màu | Dùng cho |
|---|---|---|
| `"actor"` | cam | nút bấm, người chơi |
| `"kernel"` | tím | AK Timer, scheduler |
| `"screen"` | xanh dương | Display Task, màn hình |
| `"object"` | xanh lá | task object game |
| `"store"` | xám | EEPROM, buzzer, LCD |

**Các lệnh vẽ** (gọi theo thứ tự từ trên xuống):

| Lệnh | Vẽ ra |
|---|---|
| `d.msg(a, b, "nhãn")` | Mũi tên nét liền a → b (gọi hàm trực tiếp) |
| `d.msg(a, b, "nhãn", "async")` | Mũi tên nét đứt (message post vào hàng đợi / do timer phát) |
| `d.self_(a, "nhãn")` | Mũi tên vòng về chính a (xử lý nội bộ). `d.msg(a, a, ...)` cũng tự đổi thành dạng này |
| `d.note(a, b, "chữ")` | Ghi chú màu vàng trải từ cột a tới cột b (`a == b` thì nằm trên một cột) |
| `d.frag("loop" \| "alt" \| "opt", "điều kiện")` | Mở khung UML |
| `d.else_("điều kiện")` | Đường phân nhánh trong khung `alt` |
| `d.end()` | Đóng khung gần nhất |
| `d.gap(px)` | Thêm khoảng trống |

Xuống dòng trong nhãn bằng `\n`. Khung có thể lồng nhau; mỗi `frag` phải có một `end` tương ứng, nếu thiếu script sẽ báo `unclosed fragment`.

**Ngôn ngữ:** mọi chữ trong sơ đồ (tiêu đề, participant, message, note, điều kiện) viết **tiếng Anh**, ngắn gọn, không dấu. Phần giải thích tiếng Việt đặt trong đoạn mô tả và chú thích *Hình n* của `docs/*.md`.

**Thêm sơ đồ mới:** viết hàm mới, rồi thêm một dòng vào dict `DIAGRAMS` ở cuối file:

```python
DIAGRAMS = {
    ...
    "object_11_shield": ob_shield,     # -> resources/diagrams/object_11_shield.drawio.svg
}
```

Tên khóa là tên file. Đổi tên khóa thì phải sửa cả đường dẫn `<img>` trong docs và xóa file cũ.

### 2. Bitmap (`gen_bitmaps.py`)

- Script tìm bitmap trong `screens_bitmap.cpp` theo **dòng comment ngay phía trên mảng**, đúng định dạng của image2cpp:
  ```cpp
  // 'bitmap_shield', 12x12px
  const unsigned char PROGMEM bitmap_shield [] = { ... };
  ```
  Thiếu dòng comment này (hoặc sai kích thước) thì bitmap bị bỏ qua hoặc render sai.
- Chỉ bitmap có tên trong danh sách `objects` (hàm `main()`) mới được xuất PNG. Thêm tên mới vào danh sách đó.
- GIF động `anim_enemy.gif`, `anim_boss.gif`, `anim_bang.gif` được tạo bằng hàm `animation()` trong `main()`: mỗi frame là `(tên bitmap, dx, dy, thời gian ms)`, lấy theo thứ tự `action_image` trong code (1 tick = 100 ms). Đổi số frame hay tốc độ animation trong game thì sửa danh sách này cho khớp.

### 3. Giao diện chung của sơ đồ (`seqdraw.py`)

Chỉ sửa khi muốn đổi kiểu dáng của **tất cả** sơ đồ. Các hằng số ở đầu file:

| Hằng | Ý nghĩa |
|---|---|
| `KIND_COLORS` | Màu nền / viền theo loại participant |
| `MIN_GAP` | Khoảng cách tối thiểu giữa hai cột |
| `HEAD_H` | Chiều cao ô tên participant (đủ cho 2 dòng chữ) |
| `FONT_MONO`, `FONT_SANS` | Font chữ nhãn message / ghi chú |
| `CH_MONO`, `CH_SANS` | Ước lượng độ rộng một ký tự, dùng để chừa chỗ cho nhãn |

## VI. Chụp màn hình / quay video từ board (`fb2png.py`)

`fb2png.py` lấy hình trực tiếp từ framebuffer LCD của board qua cổng serial, nên ảnh và video giống hệt màn hình thật (128x64, phóng to 4 lần thành 512x256).

### 1. Chuẩn bị (làm một lần)

1. Nạp firmware từ source hiện tại (có lệnh shell `lcd d`, `lcd v`, `lcd x`):
   ```sh
   cd ~/Documents/Space-King-Game/ak-base-kit-stm32l151/application
   make -j3
   make flash dev=/dev/ttyUSB0
   ```
2. Cắm board vào máy (cắm thẳng, không qua hub) và **đóng minicom** nếu đang mở, vì script cần dùng riêng cổng `/dev/ttyUSB0`.
3. **Quay về thư mục gốc của repo.** Mọi lệnh bên dưới đều chạy ở đây, vì cả đường dẫn script lẫn đường dẫn sau `-o` đều tính từ thư mục hiện tại:
   ```sh
   cd ~/Documents/Space-King-Game
   ```

### 2. Chụp một hình (PNG)

Bấm nút trên board để tới màn hình muốn chụp, rồi:

```sh
python3 tools/fb2png.py --port /dev/ttyUSB0 -o resources/images/screens/scr_menu_game.png
```

Kết quả: `saved resources/images/screens/scr_menu_game.png`. Game dừng khoảng 0.5 s trong lúc chụp.

### 3. Quay video (GIF động)

Đưa game tới đoạn muốn quay, chạy lệnh, rồi **chơi bình thường** bằng nút trên board trong lúc quay:

```sh
python3 tools/fb2png.py --port /dev/ttyUSB0 --record -o resources/images/screens/round1.gif       # 10 s (mặc định)
python3 tools/fb2png.py --port /dev/ttyUSB0 --record 5 -o resources/images/screens/round1.gif     # 5 s (1-60 s)
```

Kết quả: `saved resources/images/screens/round1.gif: 101 frames, 9.8 s`. Game không bị chậm khi quay: mỗi lần màn hình vẽ lại (mỗi tick 100 ms), firmware gửi một khung nén khoảng 200 byte, mất khoảng 17 ms.

### 4. Ảnh / video đang dùng trong README

Chụp lại với **đúng tên file** dưới đây thì README tự hiển thị bản mới, không cần sửa README:

| File (trong `resources/images/screens/`) | Nội dung | Lệnh |
|---|---|---|
| `scr_poster.png` | Màn Info "SPACE KING" (đầu README) | chụp hình |
| `scr_menu_game.png` | Menu, con trỏ ở Continue | chụp hình |
| `scr_menu_game_2.png` | Menu cuộn xuống, con trỏ ở Exit | chụp hình |
| `scr_setting_game.png` | Màn Setting | chụp hình |
| `scr_game_winner.png` | Màn "YOU ARE THE KING!" sau round 5 | chụp hình (mẹo: tạm giảm máu boss để tới nhanh, **không dưới 5**, rồi trả lại) |
| `scr_game_over.png` | Màn Game Over | chụp hình |
| `round1.gif` … `round5.gif` | Mỗi round khoảng 10 s (mục Gameplay Demo) | `--record` |

Thêm ảnh / video mới thì đặt tên khác, rồi thêm thẻ `<img src="resources/images/screens/<tên file>" ...>` vào README.

### 5. Tùy chọn khác

| Tùy chọn | Ý nghĩa |
|---|---|
| `--scale 4` | Hệ số phóng to (mặc định 4 → 512x256) |
| `--style oled` / `--style bw` | Nền tối giống các ảnh khác (mặc định) / đen trắng thuần |
| `--timeout 5` | Số giây chờ dump khi chụp hình |
| `dump.txt` (không có `--port`) | Đổi log minicom chứa các dump `lcd d` thành PNG: bật lưu log trong minicom (`Ctrl+A` rồi `L`), gõ `lcd d` ở mỗi màn hình, thoát minicom, rồi `python3 tools/fb2png.py dump.txt -o resources/images/screens/scr.png` (nhiều dump → `scr_1.png`, `scr_2.png`, ...) |

Lệnh shell tương ứng trên board (gõ được trong minicom): `lcd d` (dump 1 hình dạng hex), `lcd v [giây]` (bắt đầu quay), `lcd x` (dừng quay, bật lại log console).

### 6. Lưu ý

- Ghi đè file cùng tên mà không hỏi lại.
- Màn hình tĩnh (menu khi không bấm nút) chỉ vẽ lại khi có sự kiện, nên video có ít khung, mỗi khung dài hơn; đúng với thực tế.
- Trong lúc quay, log debug trên console tạm tắt. Nếu ngắt script giữa chừng (`Ctrl+C`), gõ `lcd x` trong minicom để bật lại.
- `lcd d` gửi ở chế độ blocking vì bộ đệm console chỉ 512 byte (gửi kiểu ngắt sẽ mất dữ liệu).

## VII. Lưu ý

- **Script ghi đè toàn bộ file đầu ra.** Mỗi lần chạy `gen_diagrams.py` là sinh lại cả 16 file `.drawio.svg`; mọi chỉnh sửa tay bằng draw.io trên các file đó sẽ mất. Chọn một trong hai cách và giữ nguyên:
  - sửa bằng script (khuyến nghị: nhanh, các sơ đồ luôn đồng bộ kiểu dáng), hoặc
  - sửa tay bằng draw.io và **không chạy lại** `gen_diagrams.py` nữa.
  Nếu lỡ chạy, khôi phục bằng `git checkout -- resources/diagrams/<file>`.
- **Script không đọc code game.** Sơ đồ chỉ đúng khi nội dung trong `gen_diagrams.py` được cập nhật theo code. Sau mỗi thay đổi luồng game, đối chiếu lại tên signal và interval với `app.h`.
- **Mở / sửa file `.drawio.svg`:** dùng [app.diagrams.net](https://app.diagrams.net) (File → Open) hoặc extension *Draw.io Integration* trong VSCode. Khi lưu, giữ đuôi `.drawio.svg` để GitHub vẫn hiển thị được.
- **Nhãn quá dài** làm sơ đồ rất rộng. Nên tách thành nhiều dòng bằng `\n` hoặc viết gọn (bỏ tiền tố `SC_GAME_` khi đã rõ ngữ cảnh).
- **Không commit thư mục `tools/__pycache__/`** (Python tự sinh khi chạy). Có thể thêm `__pycache__/` vào `.gitignore`.
- **Font hiển thị** trên GitHub có thể khác một chút so với máy bạn; bố cục đã chừa dư chỗ cho nhãn nên thường không bị tràn.

## VIII. Lỗi thường gặp

| Lỗi | Nguyên nhân | Cách xử lý |
|---|---|---|
| `ModuleNotFoundError: No module named 'PIL'` | Chưa có Pillow | `sudo apt install python3-pil` |
| `ModuleNotFoundError: No module named 'seqdraw'` | Đã chép `gen_diagrams.py` ra chỗ khác | Để `gen_diagrams.py` và `seqdraw.py` cùng thư mục `tools/` |
| `AssertionError: unclosed fragment in ...` | Thiếu `d.end()` cho một `frag` | Thêm `d.end()` |
| `KeyError: 'xyz'` khi chạy `gen_diagrams.py` | Dùng khóa participant chưa khai báo trong `P(...)` | Thêm participant hoặc sửa khóa |
| `KeyError: 'bitmap_...'` khi chạy `gen_bitmaps.py` | Tên bitmap không có trong `screens_bitmap.cpp`, hoặc thiếu dòng comment `// 'tên', WxHpx` | Sửa tên / thêm comment |
| `fb2png.py`: `Permission denied` / `Device or resource busy` | Thiếu quyền cổng serial hoặc minicom đang mở cổng | Thêm user vào nhóm `dialout`; đóng minicom |
| `python3: can't open file '.../tools/fb2png.py'` hoặc ảnh không hiện trong README | Chạy lệnh ở sai thư mục (ví dụ `ak-base-kit-stm32l151/application`) | `cd ~/Documents/Space-King-Game` rồi chạy lại; xóa ảnh lưu nhầm trong `application/resources/` nếu có |
| `fb2png.py`: `no complete dump found` / `no frame received` | Firmware cũ chưa có lệnh `lcd d` / `lcd v`, hoặc dump bị cắt | Build + nạp lại firmware; tăng `--timeout` |
| Console không in log nữa sau khi quay | Script bị ngắt giữa chừng nên chưa gửi `lcd x` | Gõ `lcd x` trong minicom, hoặc đợi hết thời gian quay rồi bấm một nút |
| Sơ đồ trên GitHub không cập nhật | GitHub cache ảnh | Đợi vài phút hoặc tải lại trang (Ctrl+F5) |
