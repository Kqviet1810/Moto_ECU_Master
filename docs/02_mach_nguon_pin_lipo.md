# 02 – Mạch nguồn: Li-Po 3.7V 2000 mAh, sạc qua USB-C, cách ly nguồn, đo pin

> Tra cứu datasheet đã xác nhận cho **BQ24074** (OUT = 4.4V, K_ISET = 890 A·Ω, K_ILIM = 1610 A·Ω, OVP 10.5V, EN1/EN2 không được để hở, ILIM hở = không sạc). Linh kiện khác (LM2596-5.0, LM66100, TPS63001, DW01A) theo hiểu biết datasheet – **kiểm tra lại trước khi đặt hàng**.

## 1. Kiến trúc chốt (sửa tối thiểu sơ đồ hiện có)

```
                      ┌─ giữ nguyên: F1 → D5 SS510 → C1 → U4 ─┐
12V xe ─ F1 ─ D5 ─ C1 ─► LM2596-5.0 (đổi từ -3.3, cùng chân/footprint, L1,D3,C2 giữ nguyên) ─► 5V_CAR
                                                                                              │
USB-C VBUS ─ PTC 1A ─ USBLC6 ─► LM66100 (U9) ─┐         LM66100 (U8) ◄───────────────────────┘
                                              ├─► 5V_IN ─► BQ24074 IN
                                              ┘                  │
                                         Li-Po + PCM + NTC ◄─► BAT      OUT = VSYS (4.4V khi có nguồn, = VBAT khi không)
                                                                         │
                                                      SW (công tắc trượt) ─► TPS63001 (buck-boost) ─► 3V3 ─► ESP32, TFT, SD,
                                                                                                          SN65HVD230, L9637D VCC
12V (sau F1) ──[100k/20k]── GPIO2 (VCAR)        BAT ──[100k/100k]── GPIO1 (VBAT)
BQ24074 /CHG → GPIO38, /PGOOD → GPIO39
```

Vì sao chọn vậy:
- **Chỉ cần 5V ở một chỗ** (vào BQ24074). CAN (HVD230) và K-line (L9637D VCC) đều chạy 3.3V, không cần boost 5V.
- **BQ24074 là power-path**: hệ thống chạy trực tiếp từ USB/xe, chỉ phần dư mới sạc pin → **tự ngắt khi đầy** (dòng giảm còn ~10% ISET ⇒ /CHG lên cao) kể cả khi vừa dùng vừa sạc. (TP4056 không có power-path nên không báo đầy khi đang có tải.)
- Khi xe tắt/sụt áp lúc đề máy hoặc rút USB: **pin tiếp quản tự động**, ESP32 không reset.
- TPS63001 (buck-boost) giữ 3.3V suốt dải pin 3.0–4.2V; **LDO không dùng được** (VSYS ≈ 3.5V gần 3.3V + dropout → tụt áp).
- LM2596-3.3 hiện tại **phải bỏ** (không chạy được từ pin). Giữ LM2596 nhưng bản **-5.0** (FB vẫn nối đầu ra như sơ đồ cũ) → gần như không phải vẽ lại.

## 2. Cách ly / chống chảy ngược giữa pin – USB – nguồn xe

| Nguy cơ | Biện pháp |
|---|---|
| 12V xe đảo cực, xung load-dump | F1 + D5 + TVS D4 (đã có) |
| 5V từ xe (LM2596) chảy ngược vào cổng USB / laptop | **LM66100 (U8)** chặn dòng ngược |
| 5V USB chảy ngược vào LM2596 khi xe tắt | **LM66100 (U9)** + U8 (mỗi nguồn 1 ideal diode) |
| Pin chảy ngược ra USB/xe | Bên trong BQ24074 (FET chỉ cho IN→OUT/BAT, BAT→OUT) |
| Pin xả quá/ngắn mạch | Pack **có mạch PCM** (DW01A + FS8205A) – hoặc tự thêm trên PCB nếu mua cell trần |
| Sạc ở nhiệt độ xấu | **NTC 10 kΩ (103AT)** trên pin → TS của BQ24074 (không có NTC: điện trở 10k cố định, mất bảo vệ nhiệt) |
| Sạc vô hạn khi pin hỏng | Timer an toàn TMR = 49.9 kΩ ≈ 6.65 h |
| Cắm ngược JST | Pack bán sẵn **thường ngược cực** – đo bằng đồng hồ rồi mới cắm; ghi rõ +/− trên lụa PCB |

> **Không cách ly điện (galvanic)**: mass xe – USB – pin chung. Đúng cho thiết bị chẩn đoán. Nếu nối laptop (adapter không cách ly) vào xe thấy nhiễu/vòng đất, dùng USB isolator (ADuM3160) – không cần ở bản đầu.
> Dự phòng rẻ: thay LM66100 bằng 2× SS34, nhưng sụt ~0.35V ⇒ 4.65V gần ngưỡng VIN-DPM 4.5V của BQ → dòng sạc bị hạ. Ưu tiên LM66100.

## 3. Danh sách linh kiện khối pin/nguồn

| Ký hiệu | Linh kiện | Giá trị / ghi chú |
|---|---|---|
| U4 | **LM2596-5.0** (thay -3.3) | Giữ F1, D5, D4, C1, L1 68µH, D3, C2. Thêm 10µF gốm ở 5V_CAR |
| U8, U9 | **LM66100** ×2 | VIN→VOUT; chân CE nối VIN để bật (xem datasheet). Cạnh U8/U9: 1µF |
| F2 | PTC 1A (VBUS) | cạnh cổng USB |
| ESD1 | USBLC6-2SC6 | D+/D− (VBUS tuỳ chọn) |
| U10 | **BQ24074RGTR** (VQFN-16, có thermal pad) | CE→GND; EN1→GND; EN2→10k→IN |
| C | 1–10µF ở IN; 4.7–47µF ở OUT; 4.7–47µF ở BAT | gốm X5R, sát chân |
| R_ISET | **1.78k (E96) / 1.8k** | 890/0.5A → ≈ 0.5 A (≈ 0.25C, mát, nhẹ cho pin) |
| R_ILIM | **1.8k** | 1610/0.9A → ≈ 0.9 A. Muốn an toàn cho cổng PC 500 mA: 3.24k (0.5 A) |
| R_TMR | **49.9k** | t = R·10·48/3600 ≈ 6.65 h |
| R_ITERM | để hở | mặc định 10% ISET ≈ 50 mA (muốn tuỳ biến: R = I_TERM·R_ISET/0.03) |
| TS | NTC 10k trong pack (103AT) | hoặc 10k cố định |
| R_CHG, R_PG | 100k từ /CHG và /PGOOD lên **3V3** (cho GPIO) | LED báo: LED + 1k từ **OUT(VSYS)** tới /CHG, /PGOOD (sáng khi chân kéo thấp, không tốn GPIO) |
| U11 | **TPS63001DRCR** (3.3V cố định) | VIN←SW1←OUT; L = 1.5µH; Cin 10µF, Cout 22µF+. Thay thế: TPS63802 |
| SW1 | công tắc trượt | cắt VIN của TPS63001 (tắt máy khi chỉ chạy pin); khi cắm 12V/USB nên vẫn cho sạc |
| BAT1 | JST-PH 2.0 | pin 3.7V 2000 mAh có PCM |
| R_VB1/R_VB2 | **100k / 100k** + C 100nF sát GPIO1 | từ BAT → ADC1 (VBAT) |
| R_VC1/R_VC2 | **100k / 20k** + C 100nF sát GPIO2 + **BAT54S** kẹp 3.3V | từ 12V (sau F1, **trước D5**) → ADC1 (VCAR) |
| U12 (tuỳ chọn) | MAX17048 (I2C 0x36) | % pin chính xác; nối vào SDA/SCL đã có (IO6/IO7), VDD từ BAT |

Công suất: vừa chẩn đoán vừa sạc 0.5 A lấy khoảng 0.8 W ≈ **67 mA @12V** từ xe (cầu chì/PTC 1A là dư); từ USB cần nguồn 5V ≥ 1A.

## 4. Hiệu suất / nhiệt

- 12V→5V (LM2596) ≈ 80–85 %; BQ24074 (linear, 5V→4.4V) ≈ 88 %; TPS63001 ≈ 90 % ⇒ chuỗi ≈ 65–70 %, tương đương LM2596-3.3 cũ (~75 %).
- Linear charger tản nhiệt: P ≈ (VIN−VBAT)·ICHG, ví dụ (5−3.7)·0.5A ≈ 0.65W → thermal pad nối đồng mass đủ diện tích (≥ 1 cm²).

## 5. Thời gian dùng pin

`python3 tools/battery_runtime.py` (sửa số tải ở đầu file cho khớp linh kiện thật).
Giả thiết: 2000 mAh × 0.85 = 1700 mAh ≈ 6.3 Wh; TPS63001 90 %; Iq phía pin 0.15 mA.

| Chế độ (chỉ chạy pin) | Tải @3V3 | Dòng pin | Thời gian |
|---|---|---|---|
| Chẩn đoán CAN (TFT 70 %, SD ghi log) | ~158 mA | ~160 mA | **~10.6 h** |
| Chờ / xem dữ liệu (TFT 20 %, ESP 80 MHz) | ~78 mA | ~79 mA | **~21.5 h** |
| WiFi truyền dump (TFT 50 %) | ~208 mA | ~210 mA | **~8.1 h** |
| Đèn nền 100 % (xấu nhất khi chẩn đoán) | ~180 mA | ~182 mA | **~9.3 h** |
| Màn tắt + light-sleep | ~3 mA | ~3 mA | **~22 ngày** |

(Số lấy từ output script; đèn nền TFT chiếm ~½ công suất ⇒ **điều khiển đèn nền bằng GPIO15** là cách tăng giờ dùng hiệu quả nhất.)
Sạc 0.5 A: **~4–4.5 h**. K-line cần 12V thật nên không dùng được khi chỉ chạy pin (CAN thì dùng được nếu bus có nguồn riêng).
Khi cắm 12V xe: pin chỉ là dự phòng và được giữ đầy.

## 6. Hiển thị trên TFT – 2 chân đo (yêu cầu 7)

| Đại lượng | Chân | Mạch | Dải |
|---|---|---|---|
| VBAT (→ % pin) | **GPIO1** (ADC1_CH0) | 100k/100k, ×2 | 4.2V → 2.1V |
| VCAR (báo nguồn xe) | **GPIO2** (ADC1_CH1) | 100k/20k, ×6 | 14.8V → 2.47V; TVS clamp 29V → BAT54S bảo vệ |
| /CHG | GPIO38 | | LOW = đang sạc |
| /PGOOD | GPIO39 | | LOW = có USB hoặc 12V (qua U8/U9) |

- Dùng **ADC1** (GPIO1–10); ADC2 xung đột WiFi.
- `analogReadMilliVolts()` có hiệu chuẩn eFuse; trung bình 16 mẫu; sai số ~±2%.
- Trạng thái: **đang sạc** = PGOOD & CHG thấp, **đầy** = PGOOD thấp & CHG cao, **chạy pin** = PGOOD cao.
- Cảnh báo pin yếu ở ≤ 3.4V, tự tắt (deep-sleep) ở ≤ 3.2V (PCM cắt ở ~2.5V).
- Code: [`PowerMonitor.h`](../firmware/MotoECU/PowerMonitor.h).
