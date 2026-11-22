# Hướng dẫn bắt đầu

[English](00-guide-getting-started.md) · **Tiếng Việt**

Tài liệu này hướng dẫn cài môi trường, build, nạp firmware Space King lên AK Embedded Base Kit và giới thiệu cấu trúc mã nguồn.

## I. Môi trường

| Thành phần | Ghi chú |
|---|---|
| Hệ điều hành | Ubuntu / Linux |
| Editor | VSCode (khuyến nghị) |
| Toolchain | [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) `arm-none-eabi` (đã thử với **15.3.rel2**) |
| Công cụ nạp | `ak-flash` (có sẵn trong repo) hoặc ST-Link + STM32CubeProgrammer |
| Console | `minicom` |

### Cấu hình toolchain

Mở [`ak-base-kit-stm32l151/application/Makefile`](../ak-base-kit-stm32l151/application/Makefile) và sửa `GCC_PATH` trỏ tới thư mục toolchain:

```make
GCC_PATH = $(HOME)/Documents/arm-gnu-toolchain-15.3.rel2-x86_64-arm-none-eabi
```

Các đường dẫn `LIBGCC`, `LIBGCOV`, `LIB_PATH` có chứa **phiên bản GCC** (`.../lib/gcc/arm-none-eabi/15.3.1/...`). Khi đổi toolchain phải sửa cả số phiên bản này, nếu không sẽ gặp lỗi:

```
make: *** No rule to make target '.../lib/gcc/arm-none-eabi/<version>/thumb/v7-m/nofp/libgcc.a'
```

Xem phiên bản đúng bằng: `ls $GCC_PATH/lib/gcc/arm-none-eabi/`.

## II. Build

```sh
cd ak-base-kit-stm32l151/application
make -j3
```

Kết quả nằm trong `build_ak-base-kit-stm32l151-application/`:
- `ak-base-kit-stm32l151-application.axf`: file ELF (dùng để debug)
- `ak-base-kit-stm32l151-application.bin`: file nạp vào chip

`make help` liệt kê các lệnh khác (`flash`, `com`, `debug`, `clean`, `asm`, `sym`).

## III. Nạp firmware

Firmware ứng dụng nằm ở địa chỉ **`0x08003000`**, sau bootloader (`0x08000000`) và vùng dữ liệu chung BSF (`0x08002000`).

### Cách 1: qua cổng USB với `ak-flash` (khuyến nghị)

Cần board **đã có bootloader AK** (board mua về thường đã có sẵn).

```sh
# Cài ak-flash một lần
cd ak-flash
make
sudo make install

# Nạp
cd ../ak-base-kit-stm32l151/application
make flash dev=/dev/ttyUSB0
```

Nếu `/dev/ttyUSB0` báo không có quyền, thêm user vào nhóm `dialout` (`sudo usermod -aG dialout $USER`) rồi đăng nhập lại.

### Cách 2: qua ST-Link (SWD)

Dùng khi chip còn trắng hoặc cần nạp lại bootloader. Cài STM32CubeProgrammer, sửa `PROGRAMER_PATH` trong Makefile cho đúng thư mục `bin` của nó, sau đó:

```sh
make flash
```

## IV. Console debug

```sh
make com dev=/dev/ttyUSB0      # minicom 115200 baud
```

Các log `APP_DBG(...)` / `APP_DBG_SIG(...)` trong code in ra console này.

## V. Cấu trúc mã nguồn game

```
ak-base-kit-stm32l151/application/sources/app/
├── app.h                       # Định nghĩa signal và interval timer của mọi task
├── app.cpp                     # Khởi tạo hệ thống, timer khởi động màn hình
├── app_bsp.cpp                 # Callback 3 nút MODE / UP / DOWN
├── app_eeprom.h                # Địa chỉ EEPROM, struct setting / save data
├── task_list.cpp               # Bảng task: ID, priority, handler
├── task_display.cpp            # Task hiển thị -> screen manager
├── game/spacecraft_game/       # Mỗi object game là một task riêng
│   ├── sc_game_spacecraft.*    # Tàu của người chơi
│   ├── sc_game_arrow.*         # Mũi tên của người chơi
│   ├── sc_game_enemy.*         # Enemy (round 1-3)
│   ├── sc_game_arrow_enemy.*   # Đạn của enemy
│   ├── sc_game_meteoroid.*     # Thiên thạch (round 4)
│   ├── sc_game_boss.*          # Boss (round 5)
│   ├── sc_game_boss_skill.*    # Energy ball, laser
│   ├── sc_game_gift.*          # Quà tặng (tim, sức mạnh mũi tên)
│   ├── sc_game_bang.*          # Hiệu ứng nổ
│   └── sc_game_starfield.*     # Ảnh nền
└── screens/
    ├── scr_startup / scr_info / scr_idle      # Khởi động, thông tin, màn chờ
    ├── scr_menu_game / scr_game_setting       # Menu, cài đặt
    ├── scr_game_change_round                  # Màn chuyển round
    ├── scr_spacecraft_game                    # Màn chơi chính + điều phối round / tick
    ├── scr_game_over                          # Game over
    └── screens_bitmap.*                       # Toàn bộ bitmap
```

### Thêm một object mới

1. Thêm task ID vào `task_list.h` và một dòng vào bảng `task_list.cpp` (priority `TASK_PRI_LEVEL_4` như các object khác).
2. Định nghĩa signal và interval trong `app.h` (theo mẫu `SC_GAME_..._SETUP / _RESET`).
3. Viết `sc_game_<object>.cpp/.h` với hàm `sc_game_<object>_handle(ak_msg_t* msg)`.
4. Thêm file vào `game/spacecraft_game/Makefile.mk`.
5. Gửi signal SETUP trong `scGameSpacecraftRoundSetup`, RESET trong `SC_GAME_RESET`, và các signal cập nhật trong `scGameSpacecraftTimeTick` (`scr_spacecraft_game.cpp`).
6. Vẽ object trong `view_scr_spacecraft_game()`.

## VI. Sinh lại sơ đồ và hình ảnh tài liệu

Sơ đồ sequence và ảnh bitmap trong tài liệu được sinh bằng script, chỉ cần Python 3 và Pillow:

```sh
python3 tools/gen_diagrams.py   # -> resources/diagrams/*.drawio.svg
python3 tools/gen_bitmaps.py    # -> resources/images/bitmap/*.png, *.gif
```

Các file `.drawio.svg` mở trực tiếp được bằng [draw.io / diagrams.net](https://app.diagrams.net/) để chỉnh tay. Lưu ý: chạy lại `gen_diagrams.py` sẽ ghi đè các chỉnh sửa tay, nên sau khi đã sửa bằng draw.io thì sửa tiếp bằng draw.io.

Hướng dẫn chi tiết (cách sửa nội dung sơ đồ, quy trình khi game thay đổi, lỗi thường gặp): [tools/README.md](../tools/README.md).
