# Review & sửa lỗi game Spacecraft (Space King)

Ngày: 2026-10-30, cập nhật 2026-11-03 (mục 3.12–3.18)
Phạm vi: `sources/app/game/spacecraft_game/`, `sources/app/screens/` (`scr_spacecraft_game`, `scr_menu_game`, `scr_startup`, `scr_info`), `app.h`, `app_bsp.cpp`, `app_eeprom.h`, `Makefile`.

> **Trạng thái:** tất cả thay đổi dưới đây đã build OK, **chưa test đầy đủ trên board** (xem checklist ở mục 6).

Ghi chú nền tảng giúp hiểu các lỗi về thứ tự xử lý:
- Mọi task game (`SC_GAME_*_ID`, `AC_TASK_DISPLAY_ID`) cùng `TASK_PRI_LEVEL_4` → dùng **chung một hàng đợi FIFO**. Message được xử lý đúng thứ tự post.
- Mỗi `SC_GAME_TIME_TICK` (100 ms) post một loạt message (update → detonation → check win → check game over), cuối cùng là `SC_GAME_RENDER` để vẽ khung hình (mục 3.12). Message do các handler này post thêm sẽ nằm **sau** toàn bộ loạt đó.
- `timer_remove_attr()` gỡ timer **và** xoá luôn message cùng signal đang chờ trong queue của task.
- `timer_set()` với timer đã tồn tại chỉ cập nhật `counter`, **không** cập nhật `period`.

---

## 1. Build / công cụ

| Vấn đề | Nguyên nhân | Trạng thái |
|---|---|---|
| `No rule to make target .../10.3.1/.../libgcc.a` | Đổi `GCC_PATH` sang toolchain 15.3 nhưng `LIBGCC`, `LIBGCOV`, `LIB_PATH` vẫn trỏ `10.3.1` | ✅ Sửa thành `15.3.1` trong `Makefile` |
| `make flash`: `STM32_Programmer.sh: No such file` | Máy không có STM32CubeProgrammer/ST-Link; board cắm qua CH340 (`/dev/ttyUSB0`); `ak-flash` chưa cài | ⏳ Chưa xử lý. Dùng `make flash dev=/dev/ttyUSB0` sau khi cài `ak-flash` (cần board đã có bootloader AK, app nạp ở `0x08003000`) |
| Warning `LOAD segment with RWX permissions` | binutils mới cảnh báo với linker script cũ | ℹ️ Vô hại. Có thể thêm `-Wl,--no-warn-rwx-segments` |
| Warning `memcpy` đọc 88 byte từ `_start_boot_share_data_flash` (`sys_boot.c`) | Symbol linker khai báo `uint32_t` | ℹ️ Vô hại |

---

## 2. Timer: chuyển sang `TIMER_PERIODIC`

Các timer trước đây là `ONE_SHOT` nhưng handler tự `timer_set` lại với cùng interval → đổi sang `PERIODIC`, đặt một lần lúc setup, bỏ `timer_set` trong handler.

| Timer | Đặt ở đâu |
|---|---|
| `SC_GAME_TIME_TICK` | cuối `scGameSpacecraftRoundSetup` |
| `SC_GAME_ARROW_ENEMY_SHOOT`, `SC_GAME_ENEMY_RANDOM_MOVE_Y`, `SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y` | `RoundSetup` round 2/3 (vẫn giữ `task_post_pure_msg` để lần đầu chạy ngay) |
| `SC_GAME_METEOROID_RANDOM` | `RoundSetup` round 4 |
| `SC_GAME_ARROW_ENEMY_RANDOM_SHOOT` | `scGameEnemyBuildStep` khi đội enemy round 1 xếp xong |

**Cố ý giữ `ONE_SHOT`:**
- `SC_GAME_GIFT_RANDOM_FOR_*`: chỉ đặt lại khi `total_gift_spawned < GIFT_MAX`.
- `SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL`: chu kỳ đổi theo `boss_enraged` (`timer_set` không đổi được `period`).
- `SHOOT_LASER` ↔ `RESET_LASER`: chuỗi luân phiên.
- `SC_GAME_ENEMY_BUILD_TEAM_*`: chỉ còn dùng timer 1 s khi enemy hồi sinh; việc bay vào chạy theo tick (mục 3.12).

**Về sau bỏ hẳn timer** (chuyển sang chạy theo tick, mục 3.12): `SC_GAME_ENEMY_MOVE_Y`, `SC_GAME_ENEMY_MOVE_ZIGZAG_Y`, `SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL`.

---

## 3. Lỗi đã sửa

### 3.1 Round 2/3 khởi tạo sai enemy — **nghiêm trọng**
- **File:** `sc_game_enemy.cpp`, `scGameEnemySetupMoveY`
- **Lỗi:** dùng `enemy[i]` (index team 0..3) thay vì `enemy[j]` khi gán `hp`, `hearts`, `action_image`.
- **Hậu quả:** enemy 4..9 giữ giá trị cũ (hp=2, hearts=1 sau reset). Nếu bật máy rồi chơi tiếp từ round 2/3 thì `hearts = 0` → 6 enemy không bao giờ xuất hiện, round chỉ còn 4 con.
- **Sửa:** `enemy[i]` → `enemy[j]`.

### 3.2 `remaining_meteoroids` tràn số → round 4 không kết thúc
- **File:** `sc_game_meteoroid.cpp`
- **Lỗi:** `uint16_t` bị giảm ở cả `Move` (bay khỏi màn) và `Detonation`. Thứ tự tick là `DETONATOR → CHECK_WIN → MOVE`: còn 1 → `MOVE` về 0 → tick sau `DETONATOR` giảm tiếp → 65535, `CHECK_WIN` không bao giờ đạt.
- **Sửa:** chỉ giảm khi `> 0`.

### 3.3 Thiên thạch hồi sinh với hp ≤ 0
- **File:** `sc_game_meteoroid.cpp`
- **Lỗi:** spawn chỉ đặt x, y, visible; chỉ thiên thạch bay khỏi màn mới được reset hp → thiên thạch từng bị phá sẽ chết sau một phát.
- **Sửa:** `scGameMeteoroidSpawn` reset `hp`; `scGameMeteoroidRandom` gọi lại `Spawn` (trước đó chép trùng code và `Spawn` không được dùng → warning).

### 3.4 Thắng và thua trong cùng một tick
- **File:** `scr_spacecraft_game.cpp`
- **Lỗi:** `ENEMY_CHECK_WIN`/`BOSS_CHECK_WIN` và `SPACECRAFT_CHECK_GAME_OVER_*` cùng một tick → cả `SC_GAME_ROUND_COMPLETE` và `SC_GAME_CHECK_GAME_OVER` đều chạy, cả timer `NEXT_ROUND` lẫn `EXIT_GAME` đều nổ → save bị reset về round 1 rồi lại tăng lên round 2 và ghi EEPROM.
- **Sửa:**
  - `ROUND_COMPLETE`: bỏ qua nếu `sc_game_state != GAME_PLAY`.
  - `CHECK_GAME_OVER`: bỏ qua nếu đã `GAME_OVER`; nếu không thì gỡ timer `SC_GAME_NEXT_ROUND` → **thua được ưu tiên**.

### 3.5 Timer bị rò sau `SC_GAME_RESET`
- **File:** `scr_spacecraft_game.cpp`
- **Lỗi chính:** thua trong lúc enemy round 1 đang hồi sinh/xếp hàng → chuỗi `ENEMY_BUILD_TEAM_*` chạy tiếp sau reset và bật `ARROW_ENEMY_RANDOM_SHOOT` periodic không ai gỡ → chiếm 1 slot timer và ván sau enemy round 2/3 bắn thêm kiểu random. (Không gây tiếng buzzer ngoài menu vì enemy đã ẩn.)
- **Sửa:** `SC_GAME_RESET` gỡ thêm `ENEMY_BUILD_TEAM_0..3`, `GIFT_RANDOM_FOR_METEOROID`, `SPACECRAFT_SETUP`, `SPACECRAFT_RESET_INVINCIBLE`.

### 3.6 Tàu "chết" nhiều lần trong một lần kiểm tra
- **File:** `sc_game_spacecraft.cpp`, `scGameSpacecraftHandleDeath`
- **Lỗi:** ở mạng cuối, hàm `return` trước khi bật `is_invincible`; nếu có 2 va chạm cùng lúc (2 viên đạn, hoặc energy ball + laser) thì `spacecraft_hearts` (`uint8_t`) 0 → 255 và để lại timer hồi sinh. Ít gặp, hậu quả nhẹ (255 bị `CHECK_GAME_OVER` ghi đè).
- **Sửa:** thoát ngay nếu `spacecraft.visible == BLACK` (đã chết). Từ mục 3.17 còn thoát nếu `is_hit_showing` (đang hiển thị trúng đạn).

### 3.7 Mũi tên đã bay qua vẫn trúng enemy
- **File:** `sc_game_enemy.cpp`, `scGameEnemyDetonation`
- **Lỗi:** chỉ kiểm tra `enemy.x < arrow.x + 7`, không có cận trên → round 2/3 enemy đi dọc vào hàng của một mũi tên đã ở xa phía phải vẫn bị trúng.
- **Sửa:** thêm `arrow.x < enemy.x + SIZE_BITMAP_ENEMIES_X`.

### 3.8 Gift khớp nhầm slot
- **File:** `sc_game_enemy.cpp`, `sc_game_meteoroid.cpp`, `sc_game_gift.cpp`
- **Lỗi:**
  - Khi rơi gift, mọi slot có `object_id == i` đều khớp (kể cả slot cũ) → gift đang rơi bị "dịch chuyển", hoặc slot `GIFT_NONE` bị bật thành gift tàng hình.
  - Khi hết slot trống hoặc retry 50 lần vẫn chọn trúng enemy chết, vẫn set `has_gift` và tăng `total_gift_spawned` → mất gift.
- **Sửa:**
  - Khi rơi: chỉ lấy slot `object_id == i && visible == BLACK && type != GIFT_NONE`, rồi `break`.
  - Khi phân gift: chỉ set `has_gift`/`total_gift_spawned++` khi tìm được slot trống và mục tiêu hợp lệ.

### 3.9 `rand()` chưa được seed
- **File:** `scr_menu_game.cpp`
- **Lỗi:** không có `srand()` → seed mặc định = 1 → mỗi lần bật máy diễn biến (vị trí enemy, thiên thạch, gift, hướng boss) gần như y hệt.
- **Sửa:** `srand(sys_ctrl_millis())` khi người chơi chọn vào game (item 1 và item 2 của menu).

### 3.10 Setting không được kiểm tra khi EEPROM trắng
- **File:** `app_eeprom.h`, `scr_spacecraft_game.cpp`, `scr_info.cpp`
- **Lỗi:** `arrow_speed` chỉ được gán 5 khi bấm Save ở màn Setting. Nếu chưa Save, `arrow_speed`/`num_arrow`/`level` lấy từ EEPROM có thể là 0 hoặc 0xFF → mũi tên đứng yên, không bắn được, máu enemy sai.
- **Sửa:** thêm `sc_game_setting_validate()` trong `app_eeprom.h` (num_arrow 1..10, mặc định 5; arrow_speed luôn 5; level 1..3, mặc định 1). Gọi sau khi đọc EEPROM ở màn game (`settingsetup`) và ở `scr_info` (`settingdata`).

### 3.11 Lỗi vặt / dọn code
| Mục | Sửa |
|---|---|
| `arrow.x == MAX_AXIS_X_ARROW` (chỉ đúng khi speed = 5) | `>=` |
| `case SC_GAME_ARROW_ENEMY_SHOOT` thiếu `break` | Đã thêm |
| `scGameBang*` và `sc_game_bang_display` lặp theo `TOTAL_ENEMY` | Dùng `NUM_BANG` |
| Boss dùng `SC_METEOROID_MOVE_SPEED` (define trùng ở 2 header) | Tách thành `SC_BOSS_MOVE_SPEED` |
| Comment `#endif` của `sc_game_boss.h` ghi `__SC_GAME_METEOROID_H__` | Sửa comment (guard thật vốn đúng) |
| `MAX_AXIS_X_ARROW` define trùng trong `sc_game_boss_skill.h` (không dùng) | Xoá |
| Warning `-Wsign-compare` ở `scGameEnemyBuild` (`int32_t x` so với `uint32_t target_x`) | `enemy_team_t::target_x` → `int32_t` |

### 3.12 Di chuyển chậm / khựng
- **Nút bấm** (`app_bsp.cpp`):
  - UP và DOWN dùng chung một cờ `is_hold`. Đang giữ UP mà bấm DOWN thì timer `SPACECRAFT_UP` không bị gỡ, tàu tự trôi mãi. Sửa: mỗi nút một cờ, và khi nhả luôn gỡ timer di chuyển.
  - Tàu chỉ đi (MODE chỉ bắn) khi nhả nút. Sửa: gửi ngay lúc nhấn (20 ms). Round 4 vẫn gửi lúc nhả, vì giữ nút ở round 4 nghĩa là tiến/lùi.
  - Giữ nút: chờ 250 ms + 150 ms mới đi bước đầu, timer 150 ms lệch nhịp vẽ 100 ms nên lúc đi lúc đứng. Sửa: đi 1 bước ngay khi nhận ra đang giữ, sau đó dùng timer `SC_GAME_SPACECRAFT_MOVE_INTERVAL` = 100 ms.
- **Vẽ trước khi cập nhật** (`scr_spacecraft_game.cpp`): `TIME_TICK` chỉ post message cập nhật, rồi màn hình vẽ ngay, nên mỗi khung hình là trạng thái của tick trước. Tàu còn trễ thêm một tick vì biến đệm `spacecraft_x/y`. Sửa:
  - `TIME_TICK` gọi `SCREEN_NONE_UPDATE_MASK()` để không vẽ ngay.
  - Cuối `scGameSpacecraftTimeTick()` post `SC_GAME_RENDER` cho display. Message này nằm sau toàn bộ cập nhật trong hàng đợi, và lần vẽ sau nó dùng vị trí mới nhất.
  - Bỏ biến đệm và `SC_GAME_SPACECRAFT_UPDATE`; nút bấm đổi thẳng `spacecraft.x/y`.
- **Object chạy timer riêng, lệch nhịp vẽ:**
  - Enemy round 1 bay vào (`scGameEnemyBuild`) dùng timer one-shot 100 ms tự đặt lại, mỗi bước 13 px. Sửa: `BUILD_TEAM_n` chỉ bật bit trong `building_teams`; `ENEMY_UPDATE` mỗi tick dịch 6 px (dừng đúng `target_x`). Thời gian bay vào lâu hơn khoảng gấp đôi; muốn nhanh như cũ thì tăng `SC_ENEMY_MOVE_SPEED_X`.
  - Enemy round 2/3 (`MOVE_Y` / `MOVE_ZIGZAG_Y`) và `MOVE_ENERGY_BALL` dùng timer periodic 200 ms, thực chất 5 fps. Sửa: post trong tick, enemy đi cố định 2 px/tick (`SC_ENEMY_MOVE_SPEED_Y`); energy ball đi cố định 3 px/tick (`SC_ENERGY_BALL_SPEED_X`), không còn phụ thuộc `arrow_speed`. Đi đều từng px thay vì xen kẽ (1/2 px, 2/3 px) nên tốc độ nhanh hơn trước: enemy 1,5 → 2 px/tick (+33 %, trôi hết màn hình khoảng 3 s thay vì 4 s), energy ball 2,5 → 3 px/tick (+20 %, khoảng 4,3 s thay vì 5,1 s).

### 3.13 Sau khi reboot, Silent hiện tắt tiếng nhưng vẫn có âm thanh
- **File:** `scr_startup.cpp`, `scr_info.cpp`, `app_eeprom.h`
- **Lỗi:** `scr_startup` gọi `BUZZER_Sleep(settingdata.silent)` lúc boot, khi `settingdata` chưa được đọc từ EEPROM (vẫn là 0, tức là có tiếng). `scr_info` đọc EEPROM sau đó nên màn Setting hiện biểu tượng tắt tiếng, nhưng không ai gọi lại `BUZZER_Sleep`. Ngoài ra EEPROM trắng cho `silent = 0xFF`, không phải giá trị `bool` hợp lệ.
- **Sửa:** `scr_startup` đọc và kiểm tra setting trước khi gọi `BUZZER_Sleep`; `scr_info` gọi lại `BUZZER_Sleep` sau khi đọc EEPROM; `sc_game_setting_validate()` đưa `silent` khác 0/1 về 0 (có tiếng).

### 3.14 Round 1: càng ít enemy, enemy càng ít bắn
- **File:** `sc_game_arrow_enemy.cpp` (`scGameArrowEnemyRandomShoot`)
- **Lỗi:** mỗi 2 s bốc 3 id trong cả 10 enemy, kể cả con đã chết; bốc trúng con đã chết thì mất lượt. Trung bình chỉ `3 × k / 10` phát mỗi 2 s (k = số enemy còn sống): còn 1 con thì khoảng 6,7 s mới bắn một phát.
- **Sửa:** lập danh sách enemy đang hiện, bốc `min(3, k)` con khác nhau trong danh sách đó (bốc xong thì lấy phần tử cuối đắp vào chỗ vừa bốc). Còn từ 3 con trở lên thì luôn đủ 3 phát mỗi 2 s; còn 1–2 con thì con nào cũng bắn.

### 3.15 Tàu nhấp nháy khi đang bất tử (cải tiến)
- **File:** `sc_game_spacecraft.cpp/.h`, `scr_spacecraft_game.cpp`
- **Trước:** sau khi hồi sinh tàu bất tử thêm khoảng 2 s nhưng nhìn không khác gì lúc bình thường, người chơi không biết lúc nào hết bất tử.
- **Sửa:** `is_invincible` thành biến toàn cục (khai báo `extern` trong `sc_game_spacecraft.h`); `sc_game_spacecraft_display()` đếm khung hình và bỏ vẽ tàu 2 trong mỗi 4 khung khi `is_invincible` bật (khoảng 2,5 lần/giây). `RESET_INVINCIBLE` tắt cờ thì tàu hiện bình thường.

### 3.16 Vùng trúng đạn của tàu, đạn bay hết màn hình mới ẩn
- **File:** `sc_game_spacecraft.cpp/.h`, `sc_game_arrow_enemy.cpp`, `sc_game_boss_skill.cpp`
- **Trước:** đạn enemy / energy ball bị ẩn ngay khi đầu đạn chạm mép trái (`x < 0`), phần đuôi biến mất giữa chừng. Điều kiện trúng chỉ có cận trên (`x + 10 <= tàu.x + 15`), không có cận dưới: đạn đã bay qua tàu mà tàu di chuyển đụng vào vẫn chết.
- **Sửa:**
  - Đạn chỉ ẩn khi ra hết khỏi màn hình (`x + SIZE_BITMAP_*_X <= 0`).
  - Thêm `scGameSpacecraftShotHitX()`, với `pass_x` = mũi tàu − 2/3 chiều ngang tàu (`SPACECRAFT_SHOT_PASS_X` = 15 × 2/3 = 10 px). Lưu `y` của tàu ở lần kiểm tra trước (`last_check_y_enemy`, `last_check_y_energy_ball`) để phân biệt:
    - **Trực diện** (tàu đã nằm trên đường đạn từ tick trước): trúng khi đầu đạn vượt qua `pass_x` ngay trong tick này (`x <= pass_x < x + step`), tức đạn vào tàu 10 px như trước đây.
    - **Tàu di chuyển vào đường đạn:** trúng nếu đạn chồng lên tàu (`x < mũi tàu`) và **đuôi** đạn chưa qua `pass_x` (`x + rộng_đạn > pass_x`; rộng 10 px với đạn enemy, 8 px với energy ball); đuôi đã qua 2/3 thì không trúng. Vì điều kiện trực diện chỉ đúng đúng một tick lúc đạn vượt `pass_x`, viên đạn đã qua 2/3 khi tàu lách vào cũng không bị tính trúng ở các tick sau.
  - `step` là quãng đạn vừa đi trong tick: `arrow_speed` với đạn enemy, `SC_ENERGY_BALL_SPEED_X` (3 px/tick, `sc_game_boss_skill.h`) với energy ball. Cách này dựa vào việc `ARROW_ENEMY_RUN` / `MOVE_ENERGY_BALL` được post trước signal kiểm tra trong cùng tick.
  - Áp dụng cho cả đạn enemy (round 1–3) và energy ball (round 5). Đã mô phỏng: đạn bay thẳng vào trúng ở độ sâu 10–14 px như cũ; tàu lách vào khi đầu đạn enemy ở x = −4…14 (đuôi 6…24) thì chết, ở x ≤ −5 (đuôi ≤ 5) thì không.

### 3.17 Hiển thị tàu chồng lên viên đạn khi trúng đạn (cải tiến)
- **File:** `app.h`, `sc_game_spacecraft.cpp/.h`, `sc_game_arrow.cpp`, `scr_spacecraft_game.cpp`
- **Trước:** trúng đạn là tàu bị ẩn và đưa về vị trí đầu ngay trong tick đó, khung hình tiếp theo đã không còn tàu nên người xem không thấy tàu va vào đạn.
- **Sửa:**
  - `scGameSpacecraftHandleDeath()` chỉ bật `is_hit_showing` và hẹn `SC_GAME_SPACECRAFT_HIT_DONE` sau `SC_GAME_SPACECRAFT_HIT_SHOW_INTERVAL` = 300 ms (khoảng 3 khung hình). Phần ẩn tàu, trừ tim, chia đôi arrow power, game over / hồi sinh chuyển sang `scGameSpacecraftHitDone()`.
  - Trong 300 ms đó tàu đứng yên tại chỗ trúng (UP/DOWN/FORWARD/BACKWARD và bắn bị chặn), các lần trúng khác bị bỏ qua.
  - `SC_GAME_ROUND_COMPLETE` bị bỏ qua khi `is_hit_showing` để giữ quy tắc "thua được ưu tiên" (mục 3.4): kiểm tra thắng lặp lại mỗi tick nên sau `HIT_DONE`, nếu còn tim thì vẫn qua round. Tim chỉ bị trừ ở `HitDone()`, nên không có trường hợp qua round với 0 tim.
  - `SC_GAME_RESET` gỡ thêm timer `SC_GAME_SPACECRAFT_HIT_DONE`; `SPACECRAFT_RESET` tắt `is_hit_showing`.
  - Áp dụng cho mọi kiểu trúng (đạn enemy, energy ball, thiên thạch, laser) vì cùng đi qua `HandleDeath()`.
- **Hệ quả:** mốc hồi sinh và hết bất tử lùi thêm 300 ms tính từ lúc trúng.

### 3.18 Mũi tên của tàu bay hết màn hình mới ẩn
- **File:** `sc_game_arrow.h`, `sc_game_enemy.cpp`, `sc_game_meteoroid.cpp`
- **Trước:** mũi tên bị ẩn khi `x >= 120`, lúc phần lớn mũi tên vẫn còn trên màn hình (màn hình rộng 128 px).
- **Sửa:** `MAX_AXIS_X_ARROW` 120 → 128 (bề ngang LCD): mũi tên chỉ ẩn và trả lại `num_arrow` khi đã ra hết mép phải. Vì mũi tên giờ bay tới sát mép, `ENEMY_DETONATOR` và `METEOROID_DETONATOR` bỏ qua object có `x >= 128` (enemy đang bay vào ở round 1, thiên thạch trôi ra mép phải) để không bắn trúng thứ chưa nhìn thấy.
- **Hệ quả:** mũi tên bắn trượt về lại kho chậm hơn khoảng 2 tick.

---

## 4. Nhận định đã được kiểm tra lại và **bác bỏ**

Để sau này khỏi nhầm:
- *"Đạn địch / energy ball đã bay qua tàu vẫn giết tàu"*: lúc review đầu không xảy ra vì đạn bị ẩn khi `x < 0`. Sau mục 3.16 (đạn bay hết màn hình mới ẩn) thì đã thêm cận dưới cho vùng trúng.
- *"Timer rò làm buzzer kêu ngoài menu"*: sai, `scGameEnemyShoot` cần enemy `visible`.
- *"Slot gift của thiên thạch bay khỏi màn bị giữ mãi"*: chỉ giữ tới khi thiên thạch cùng id xuất hiện lại và bị phá.
- *"`GiftOfBoss` ghi đè slot đã đặt trước"*: round 5 không có slot đặt trước.

---

## 5. Điểm yếu còn lại (chưa sửa)

- **Boss máu < 5 làm treo game** (`sc_gameBossHit`): bước thả quà `hp_max * 20 / 100` bằng 0 → vòng `while` không thoát. Không xảy ra với máu mặc định `level * 50`; chỉ gặp khi sửa tay máu boss (ví dụ để test). Cách sửa nếu cần: bước tối thiểu 1.

- **Kiến trúc tick:** mỗi tick post ~10 message riêng lẻ, thứ tự update/va chạm phụ thuộc thứ tự queue (gốc của lỗi 3.4, 3.6). Nên gom update + va chạm vào một hàm gọi trực tiếp mỗi tick.
- **`settingsetup.num_arrow` vừa là cấu hình vừa là bộ đếm khi chơi:** chỉ đúng nhờ đọc lại EEPROM mỗi lần vào màn. Nên tách biến runtime.
- **Nhặt gift khi tàu đang ẩn** (`scGameGiftCheckReceive`): điều kiện nhận gift không kiểm tra `spacecraft.visible` / `is_hit_showing`. Trong 1 s chờ hồi sinh, tàu ẩn ở vị trí đầu (`x = 0, y = 20`) vẫn có thể "nhặt" gift bay ngang qua; trong 0,3 s hiển thị trúng đạn cũng vậy. Cách sửa nếu cần: thêm `spacecraft.visible == WHITE && !is_hit_showing` vào điều kiện.
- **Round 1: enemy hồi sinh làm bắn thêm một loạt** (`scGameEnemyBuildStep`): enemy hồi sinh bay vào lại qua chuỗi `BUILD_TEAM`, tới đội cuối thì code xử lý như lúc xếp đội hình xong: post ngay `ARROW_ENEMY_RANDOM_SHOOT` và `timer_set` lại timer periodic 2 s (đếm lại từ đầu). Mỗi lần một enemy vào lại chỗ có thêm một loạt bắn ngoài nhịp 2 s. Cách sửa nếu cần: chỉ bắt đầu bắn ở lần xếp đội hình đầu tiên (thêm cờ).
- **Hiếm: tick đến sau khi đã thắng round** (`SC_GAME_TIME_TICK`): handler không kiểm tra `sc_game_state`. Nếu một tick xử lý lâu hơn 100 ms (ví dụ đang quay video bằng `lcd v`), tick kế tiếp có thể vào hàng đợi trước `SC_GAME_RESET` và tàu vẫn có thể bị tính trúng đạn sau khi đã qua round. Cách sửa nếu cần: `if (sc_game_state != GAME_PLAY) break;` ở đầu `case SC_GAME_TIME_TICK`.
- **Gift/heart/arrow power không có giới hạn trên** (`uint8_t`, tràn ở 255; khó đạt).
- **Warning còn lại:** `appMBMasterWrite` không dùng (ngoài code game).

---

## 6. Checklist test trên board

- [ ] Round 1: enemy bay vào xếp hàng, bắn random; thua trong lúc enemy đang hồi sinh → chơi lại tới round 2, enemy không bắn kiểu random thừa.
- [ ] Reset board, chơi tiếp từ round 2/3 → đủ 10 enemy, mỗi con đúng `hp`/`hearts` theo level.
- [ ] Round 2/3: mũi tên chỉ trúng enemy khi thật sự chạm.
- [ ] Round 4: chơi tới khi `MT` về 0 → qua round; thiên thạch xuất hiện lại cần đủ số phát mới vỡ.
- [ ] Gift rơi đúng chỗ enemy/thiên thạch bị phá, không có gift "nhảy" vị trí.
- [ ] Round 5: boss di chuyển bình thường, energy ball/laser, gift theo % máu boss.
- [ ] Reset board vài lần → thứ tự thiên thạch/gift khác nhau mỗi ván.
- [ ] Thua ở mạng cuối → màn Game Over, save về round 1.
- [ ] Bấm UP/DOWN: tàu nhích ngay khi nhấn; giữ nút: tàu chạy đều, không giật; vừa giữ UP vừa bấm DOWN rồi nhả cả hai → tàu đứng yên.
- [ ] Round 1: enemy bay vào mượt, dừng đúng hàng; round 2/3: enemy trôi đều; round 5: energy ball bay đều.
- [ ] Bật Silent, Save, reset board → không có tiếng từ màn khởi động, màn Setting hiện biểu tượng tắt tiếng; tắt Silent, Save, reset → có tiếng.
- [ ] Round 1: diệt còn 1–2 enemy → chúng vẫn bắn đều mỗi 2 s.
- [ ] Mất 1 mạng (còn mạng) → tàu hiện lại sau 1 s, nhấp nháy khoảng 2 s, rồi hiện bình thường; trong lúc nhấp nháy trúng đạn không mất mạng.
- [ ] Round 1–3 / 5: đạn bay hết mép trái mới biến mất; đạn đã qua quá 2/3 tàu thì di chuyển tàu đụng vào không chết; đạn chưa qua 2/3 thì đụng vào vẫn chết.
- [ ] Trúng đạn (đứng yên hoặc lách vào) → tàu đứng yên chồng lên viên đạn khoảng 0,3 s rồi mới biến mất; trúng đạn cùng lúc tiêu diệt enemy/boss cuối ở mạng cuối → Game Over.
- [ ] Bắn trượt → mũi tên bay ra hết mép phải mới biến mất, số `Ar` tăng lại; round 1 enemy đang bay vào (chưa hiện) không bị bắn trúng.
- [ ] Xoá vùng setting trong EEPROM (hoặc board mới chưa từng Save setting) → vào game vẫn bắn được, mũi tên bay bình thường, màn Setting hiện giá trị hợp lệ.
