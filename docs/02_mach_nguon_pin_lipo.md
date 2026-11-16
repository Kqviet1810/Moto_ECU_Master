# 02 – Mạch nguồn, sạc/xả Li-Po 3.7V 2000 mAh, cách ly nguồn

> Số liệu linh kiện lấy theo hiểu biết datasheet – **kiểm tra lại trên datasheet bản mới nhất trước khi vẽ PCB** (đặc biệt các hệ số ISET/ILIM/TMR của BQ24074).

## 1. Kiến trúc: mọi nguồn vào đều đi qua BQ24074 (power-path)

```
12V xe ─[PTC]─[chống ngược]─[TVS]─► Buck 5V ─► LM66100 ─┐
                                                        ├─► 5V_IN ─► BQ24074 ─► VSYS (3.0–4.4V) ─┬► Buck-boost 3V3 ─► ESP32-S3, TFT, SD
USB-C 5V ─[PTC]─[ESD]─► LM66100 ───────────────────────┘          │ ▲                          └► Boost 5V + load switch ─► CAN, K-line
                                                                  │ │
                                                              Li-Po + PCM + NTC
```

Vì sao chọn kiểu này:
- **Một chỗ duy nhất** hợp nguồn (5V_IN) → chỉ cần 2 ideal diode. Đơn giản, ít lỗi.
- BQ24074 là **power-path**: cấp hệ thống trực tiếp từ USB/xe và chỉ lấy phần dư để sạc pin → **sạc đầy rồi tự ngắt** đúng chuẩn, vừa chạy vừa sạc vẫn nhận biết "đầy" chính xác. (TP4056 không có power-path: tải làm dòng sạc không xuống ngưỡng cắt → không bao giờ báo đầy.)
- Pin **không bao giờ chạm 12V xe**; 12V chỉ vào buck, chia áp đo và chân VS của K-line.

## 2. Cách ly / chống chảy ngược giữa các nguồn

| Nguy cơ | Biện pháp |
|---|---|
| 12V xe (buck) ↔ USB cắm cùng lúc | **LM66100** ở mỗi nguồn: chặn dòng ngược khi nguồn kia cao hơn, sụt áp chỉ ~0.1V |
| Pin chảy ngược ra USB/xe | BQ24074 chặn (IN→OUT một chiều), cổng USB không bị cấp ngược |
| 12V đảo cực | P-MOS chống ngược (hoặc SS34) + TVS + PTC |
| Xung load-dump | TVS SMBJ18A, buck chịu ≥32V |
| Pin xả cạn / ngắn mạch | Pack **có mạch PCM** (DW01A+8205A) hoặc thêm DW01A+FS8205A trên PCB |
| Nhiệt độ sạc | **NTC 10 kΩ** trên pin → chân TS của BQ24074 (không có NTC thì gắn điện trở 10 kΩ cố định, nhưng mất bảo vệ nhiệt) |
| Cắm nhầm JST | Dùng JST-PH 2.0, đánh dấu cực rõ trên PCB (pack bán sẵn thường **ngược cực**, kiểm tra bằng đồng hồ trước khi cắm) |

> **Không** cách ly điện (galvanic). Mass xe, USB, pin chung nhau (đúng cho thiết bị chẩn đoán). Nếu cắm laptop dùng nguồn không cách ly trong lúc nối xe và thấy nhiễu/vòng đất, dùng USB isolator (ADuM4160) – không cần cho bản đầu.

## 3. Danh sách linh kiện đề xuất

| Khối | Linh kiện | Thông số chính |
|---|---|---|
| Bảo vệ 12V | PTC 0.75A, SS34 hoặc P-MOS (AO3401/SI2301), **SMBJ18A**, ferrite | chịu ngược cực, kẹp ~29V |
| Buck 12V→5V | **AP63205** (3.8–32V, 2A) hoặc LMR51430/TPS54202 | 5V ổn áp, ≥1A |
| OR-ing | **LM66100 ×2** | 1.5–5.5V, 1.5A, chặn dòng ngược |
| Sạc + power-path | **BQ24074** (hoặc BQ24072/75 tuỳ Vout) | OVP 6.6V, sạc ≤1.5A, CC/CV 4.2V, tự ngắt, /CHG, /PGOOD |
| Bảo vệ pin | PCM trong pack, hoặc **DW01A + FS8205A** | cắt quá xả (~2.4–2.5V), quá dòng, ngắn mạch |
| 3V3 | **TPS63001** (cố định 3.3V, 1.2A) hoặc TPS63802 | buck-boost: VSYS 3.0–4.4V vẫn ra đúng 3.3V |
| 5V truyền thông | **TPS61023** + load switch **TPS22918** | boost 1A, bật/tắt bằng GPIO40 |
| USB | **USBLC6-2SC6**, 2× 5.1 kΩ CC, PTC 1A | ESD + CC |
| Đo | 2× (R chia áp + 100 nF) | xem docs/01 mục 3.8 |

**Tại sao không dùng LDO (AMS1117/AP2112)?** VSYS khi pin 3.3–3.6V thấp hơn 3.3V + dropout → LDO tụt áp, ESP32 reset. Buck-boost giữ 3.3V suốt dải pin 3.0–4.2V và hiệu suất ~90% (LDO từ 4V xuống 3.3V chỉ ~82%, từ 5V chỉ 66%).

## 4. Đặt thông số BQ24074

| Thông số | Giá trị đề xuất | Cách tính (xác nhận với datasheet) |
|---|---|---|
| Dòng sạc ISET | **0.5 A** (0.25C — nhẹ cho pin, mát) | R_ISET ≈ K_ISET / I = 890 / 0.5 ≈ **1.8 kΩ** |
| Giới hạn dòng vào ILIM | **~0.9 A** | R_ILIM ≈ K_ILIM / I = 1610 / 0.9 ≈ **1.8 kΩ**. Cổng USB PC có thể chỉ 500 mA → BQ tự hạ dòng sạc (DPPM), không sao |
| Điện áp nạp đầy | 4.20 V (cố định) | |
| Ngắt khi đầy | Dòng giảm còn ~10% ISET (≈50 mA) → chip **tự ngắt**, /CHG lên cao | firmware: `full = PGOOD && !CHG` |
| Timer an toàn | **~6–8 h** | chọn R_TMR theo bảng datasheet; ngăn sạc vô hạn nếu pin hỏng |
| NTC (TS) | 10 kΩ β=3435 | cắt sạc khi <0 °C hoặc >45 °C |
| EN1/EN2 | Chế độ điện trở (ILIM/ISET) | có thể đưa ra jumper chọn 500 mA / 900 mA |

Mạch sạc cho phép **cắm USB hoặc 12V xe là sạc**; dòng nạp dùng chung 1 mạch nên không cần chia chế độ.

## 5. Ước tính thời gian dùng pin

Chạy `python3 tools/battery_runtime.py` (sửa số tải ở đầu file cho khớp linh kiện thật).

Giả thiết: pin 2000 mAh × **0.85** (cắt 3.2V, lão hoá, nhiệt) = **1700 mAh ≈ 6.3 Wh**; buck-boost 90%, boost 88%.

| Chế độ | Tải | Dòng pin | Thời gian |
|---|---|---|---|
| Chẩn đoán (TFT 70%, CAN/K-line chạy, ghi SD) | 168 mA@3V3 + 20 mA@5V | ~197 mA | **~8.6 h** |
| Chờ / xem dữ liệu (TFT 20%) | 78 mA@3V3 | ~77 mA | **~22 h** |
| WiFi truyền dump (TFT 50%) | 208 mA@3V3 | ~206 mA | **~8.2 h** |
| Standby (màn tắt, light-sleep) | 3 mA@3V3 | ~3 mA | ~22 ngày |

Sạc 0.5 A: **~4–4.5 h**; sạc 1 A: ~2.5–3 h (cần ILIM ≥ 1.4 A và cổng USB đủ mạnh).
Đèn nền TFT chiếm ~½ công suất → hạ độ sáng hoặc tự tắt màn là cách tăng thời gian dùng pin hiệu quả nhất.

## 6. Ràng buộc quan trọng cần bạn xác nhận

1. **ECU luôn cần 12V thật** (từ xe hoặc nguồn bàn) để chạy. Li-Po chỉ nuôi **thiết bị** (xem log, sửa file, WiFi, CAN trên bàn khi ECU có nguồn riêng), **không** cấp nổi cho ECU.
2. **L9637D (K-line) cần VS ≥ 6V** → chỉ dùng K-line khi có 12V ở chân VS. CAN chỉ cần 5V logic nên **chạy được bằng pin**. Nếu muốn K-line cả khi bàn thử không có 12V chung, thêm boost 3.7V→12V nhỏ (cho VS/kéo K-line) – nói tôi nếu cần.
3. Trên xe, nguồn cấp thiết bị vẫn đi qua buck → BQ24074 (pin được giữ đầy và là nguồn dự phòng khi xe sụt áp lúc đề máy).

## 7. Hiển thị trên TFT (yêu cầu 7)

`firmware/MotoECU/PowerMonitor.h` đọc 2 chân ADC + 2 chân trạng thái và trả về `PowerStatus`:
`vbat`, `percent`, `vcar`, `carPresent`, `usbOrCar`, `charging`, `full` → vẽ biểu tượng pin, "ĐANG SẠC / ĐẦY", "XE: 12.6V".
