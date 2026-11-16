# 01 – Đánh giá sơ đồ nguyên lý Rev 1.0 (2026-11-12, KQV)

Nguồn: [`schematic/Schematic_Moto-ECU-Master_rev1.0_2026-11-16.pdf`](schematic/Schematic_Moto-ECU-Master_rev1.0_2026-11-16.pdf) (1 trang, EasyEDA).
Đã đọc từng chân trên sơ đồ và đối chiếu datasheet **L9637D, BQ24074, SN65HVD230** (tra cứu online). Các linh kiện còn lại (LM2596, TPS63001, LM66100, TVS…) theo hiểu biết datasheet – **xác nhận lại trước khi đặt hàng PCB**.

> Sửa so với bản tài liệu cũ trong repo (viết khi chưa có sơ đồ): L9637D **không** cần VCC 5V / VS ≥ 6V / dịch mức. Datasheet: VCC 3–7V, TX VIH min 2.5V, VS 4.5–36V, pin K chịu −24V…VS. Phần K-line của bạn dùng 3.3V là **hợp lệ**.

---

## 1. Sơ đồ chân thực tế đọc từ schematic (ESP32-S3-WROOM-1-N4R2)

| GPIO | Net | Nối tới | Đánh giá |
|---|---|---|---|
| 4 | TX_Can | U7 SN65HVD230 pin 1 (D) | ✅ TWAI TX |
| 5 | RX_Can | U7 pin 4 (R) | ✅ TWAI RX |
| 6 / 7 | SDA / SCL | U6 AT24C02, R46/R48 4.7k lên 3V3 | ✅ |
| 9 | G9 | SD1 CS | ✅ |
| 10 | G10 | TFT CS | ✅ (FSPICS0) |
| 11 | G11 | MOSI (TFT, Touch, SD) | ✅ FSPID |
| 12 | G12 | SCK (TFT, Touch, SD) | ✅ FSPICLK |
| 13 | G13 | MISO (TFT SDO, Touch SDO, SD) | ✅ FSPIQ, ⚠️ xem 3.6 |
| 14 | G14 | Touch T_CS | ✅ |
| 8 | G8 | Touch T_IRQ | ✅ |
| 16 | G16 | TFT DC | ✅ |
| 18 | G18 | TFT RESET | ✅ |
| 17 | TX_KL | U1 L9637D pin 4 (TX) | ✅ |
| 21 | RX_KL | U1 pin 1 (RX) | ✅ |
| 19 / 20 | D− / D+ | USB-C | ✅ |
| 43 / 44 | TXD0 / RXD0 | Header UART 4 chân | ✅ |
| EN | EN | R36 10k + C8 1µF + nút reset | ✅ |
| **0** | — | **không nối (X)** | ❌ thiếu nút BOOT |
| 1, 2 | — | không nối | **rảnh – ADC1, dùng cho 2 chân đo** |
| 3, 45, 46 | — | không nối | strap – giữ nguyên, đừng dùng |
| 15, 35–42, 47, 48 | — | không nối | rảnh (module N4R2 PSRAM quad nên 35–37, 47, 48 dùng được) |

Chọn module/chân đúng: SPI dùng chân IOMUX mặc định (chạy được 40–80 MHz), tránh được chân strap và chân flash/PSRAM.

---

## 2. Những chỗ ĐÚNG (giữ nguyên)

- USB-C: R45/R47 **5.1 kΩ** ở CC1/CC2 → sạc C-to-C có nguồn; D+/D− của cả hai phía lật nối chung; GND nối đủ.
- Chân EN: R36 10k + C8 1µF đúng chuẩn; có nút reset.
- Header UART 3V3/TX/RX/GND (TX→TXD0, RX→RXD0): đúng chiều.
- CAN: SN65HVD230 3.3V nối trực tiếp GPIO không cần dịch mức; **RS (pin 8) nối GND** = chế độ high-speed; decoupling C4 100nF.
- K-line: pinout L9637D khớp datasheet (1 RX, 2 LO, 3 VCC, 4 TX, 5 GND, 6 K, 7 VS, 8 LI). VCC 3.3V trong dải 3–7V, TX 3.3V đạt VIH ≥ 2.5V, RX ra 3.3V. R2 510Ω kéo K lên VS đúng chuẩn ISO 9141 (datasheet: ≤ 5 kΩ). VS nối 12V trước cầu chì được chấp nhận vì L9637D chịu đảo cực đến −24V.
- Nguồn 12V: F1 → TVS **D4 1.5KE18CA (hai chiều)** → D5 SS510 chống ngược → C1 220µF/50V → LM2596. D3 SS510 (catch), L1 68µH, C2 470µF đúng topology buck; ON/OFF nối GND (luôn bật); FB nối đầu ra (bản 3.3V cố định). LED nguồn R1 1k.
- Các chân SPI TFT/Touch/SD dùng chung bus, mỗi thiết bị CS riêng.

---

## 3. LỖI / THIẾU – ưu tiên sửa trước khi vẽ PCB

### 🔴 Nghiêm trọng (chặn chức năng)

| # | Vấn đề | Vì sao | Cách sửa |
|---|---|---|---|
| 1 | **VBUS của USB2 không nối** (cả 2 cặp A4B9/B4A9 đánh dấu X) | Cắm USB không có nguồn, không sạc, không nhận biết cắm cáp. Nạp code chỉ chạy khi đang có 12V | Nối VBUS → PTC 1A + ESD → ideal diode (xem doc 02) |
| 2 | **GPIO0 (IO0) không nối → không có nút BOOT** | ESP32-S3 USB gốc không có mạch tự reset DTR/RTS; lần đầu hoặc khi firmware treo phải giữ BOOT + reset mới vào được chế độ nạp | Thêm nút GPIO0→GND (+ 10k lên 3V3 tuỳ chọn). Giữ header UART để cứu máy |
| 3 | **GND và GNDS là 2 tên net khác nhau** | USB, R45/R47, C6/C7/C8, nút reset nằm trên **GNDS**; ESP32, TFT, CAN, K-line, LM2596 nằm trên **GND**. Nếu trong EasyEDA không có điểm nối, hai mass bị tách → USB/EN/decoupling không có đường về | Kiểm tra netlist; thống nhất 1 tên **GND** (hoặc net-tie/0Ω ngay tại chân GND module). PDF không thể chứng minh chúng được nối |
| 4 | **Chưa có pin, sạc, nguồn dự phòng; 3.3V chỉ lấy từ LM2596 ← 12V** | LM2596 là buck (cần Vin ≳ 5V): Li-Po 3.7V không chạy được; không có chỗ cắm USB cấp nguồn | Xem doc 02: LM2596-**5.0** → ideal-diode OR cùng USB → BQ24074 → TPS63001 3.3V |
| 5 | **Không có đầu nối vào/ra**: 12V in, DLC (K, CANH, CANL, GND) chỉ là net label; **CN1 2×6 không nối chân nào** | PCB không có chỗ cắm xe/ECU | Gán CN1 hoặc thêm đầu nối DLC/ECU (đề xuất ở mục 5) |
| 6 | Không có đường **đo nguồn** (yêu cầu 7) | Màn TFT không hiện được pin / điện áp xe | GPIO1 = VBAT, GPIO2 = VCAR (doc 02 mục 6) |

### 🟠 Quan trọng (độ tin cậy / hư hỏng ngoài xe)

| # | Vấn đề | Cách sửa |
|---|---|---|
| 7 | **CANH/CANL trần, không bảo vệ.** SN65HVD230 chỉ ESD 16kV, dải điện áp bus hẹp (cỡ −4…16V theo datasheet – xác nhận) → xung/jump-start 24V xe hoặc nhầm đầu nối có thể làm hỏng | TVS CAN (PESD2CAN / ESDCAN24), choke chế độ chung tuỳ chọn. Muốn bền hơn: đổi sang transceiver chịu ±58V (TJA1051T/3 cần VCC 5V, hoặc loại 3.3V ±58V như TCAN332 – xác nhận) |
| 8 | **Không có điện trở 120Ω CAN** | Đừng hàn cứng (xe đã có 2 điện trở). Thêm **jumper hàn 120Ω** mặc định hở để test trên bàn |
| 9 | **Chân K_LINE không có TVS/ESD** | Thêm TVS 24–36V (SMBJ24A / PESD36) ở pin K; R2 dùng loại ≥ 1W (14.5V/510Ω = 0.41W khi K bị kéo thấp/ngắn mạch GND, 0805 quá tải) |
| 10 | **Thiếu tụ decoupling**: U1 (VCC & VS), U6 (VCC), SD1, TFT | 100nF sát chân U1.3, U1.7 (thêm 1µF), U6.8; 10–47µF + 100nF gần SD1 (thẻ nhớ giật 100mA+) |
| 11 | **CS TFT/SD/Touch không có pull-up** | 10k lên 3V3 trên G9, G10, G14: khi ESP32 reset/boot các CS nổi sẽ kích hoạt cả 3 thiết bị cùng lúc trên bus chung. Thẻ SD SPI nên có thêm pull-up 10k ở MOSI/MISO |
| 12 | **MISO của TFT (pin 9) chung bus với SD và Touch** | Nhiều module ILI9341 không tri-state SDO khi CS cao → làm sai dữ liệu đọc SD. TFT_eSPI không cần đọc MISO của TFT: bỏ nối pin 9 (hoặc để jumper 0Ω) |
| 13 | **Đèn nền TFT (LED pin 8) nối thẳng 3V3** | Luôn sáng 100% = tốn pin nhất (~½ công suất). Kéo về GPIO15 (PWM) – nếu module có transistor đèn nền (loại MSP2807) thì nối trực tiếp, nếu không thì qua MOSFET |
| 14 | **Không có ESD cho D+/D−** | USBLC6-2SC6 sát cổng Type-C |
| 15 | F1 chưa ghi thông số | PTC/cầu chì 1A (thiết bị chỉ lấy ~70 mA@12V khi vừa chạy vừa sạc pin) |

### 🟡 Nhỏ

| # | Vấn đề | Cách sửa |
|---|---|---|
| 16 | U6 AT24C02 chân **WP (7) để hở (X)** | Nối GND (cho ghi) – chống nhiễu |
| 17 | Nút reset đặt tên **R3** (ký hiệu điện trở) | Đổi thành SW1; thêm SW2 = BOOT |
| 18 | LI (pin 8) và LO (pin 2) của L9637D không dùng | OK vì datasheet có trạng thái định nghĩa khi hở; để nguyên |
| 19 | Ghi chú "Company" bị lỗi font | Chỉ là chữ trên khung tên (tiếng Việt có dấu) |
| 20 | Không có test point / lỗ bắt vít | TP: 3V3, 5V_CAR, VSYS, VBAT, GND, CANH, CANL, K, GPIO0, EN, TX0, RX0 |
| 21 | Anten WiFi/BLE | Keep-out ≥ 15 mm, module chìa ra mép PCB, không đổ đồng dưới anten |
| 22 | Module N4R2 = flash **4 MB** | Đủ cho firmware + SPIFFS nhỏ; dump ECU lưu SD. Cần nhiều hơn thì dùng N16R2 (cùng chân) |

---

## 4. Đọc/ghi ECU – chức năng còn thiếu về phần cứng

K-line/CAN mới chỉ đủ **chẩn đoán**. Ghi/đọc flash ECU thường còn cần (cho tôi biết đời xe/MCU ECU để chốt):
- **Công tắc nguồn cấp ECU** (P-MOS high-side ≥ 5A + cầu chì) vì phải ngắt/cấp lại nguồn ECU theo trình tự vào bootloader. GPIO40 = ECU_PWR_EN.
- **Chân điều khiển mở-collector** tới ECU (BOOT/MODE, RESET) qua 2N7002: GPIO41, GPIO42.
- **Header BDM/JTAG/UART** 2.54 mm cho đọc trực tiếp trên bàn.
- Pin Li-Po **không** cấp được ECU (ECU cần 12V thật). Pin chỉ nuôi thiết bị.

## 5. Đề xuất gán CN1 (2×6, 2.54 mm) – xác nhận lại theo đầu nối bạn chọn

| Chân | Tín hiệu | Chân | Tín hiệu |
|---|---|---|---|
| 1 | 12V_IN (từ xe) | 2 | GND |
| 3 | K_LINE | 4 | GND |
| 5 | CANH | 6 | CANL |
| 7 | 12V_ECU (qua P-MOS) | 8 | GND |
| 9 | ECU_BOOT | 10 | ECU_RST |
| 11 | 3V3 | 12 | ECU_UART_TX/RX (dự phòng) |

Đầu vào 12V của xe (DLC Honda 2P/3P, Yamaha 4P) nên ra đầu nối chắc chắn riêng; CN1 làm cổng mở rộng.

## 6. Tóm tắt thứ tự làm

1. Thống nhất GND/GNDS, nối VBUS, thêm BOOT.
2. Thêm khối pin/sạc (doc 02), đổi LM2596-3.3 → LM2596-5.0 (cùng footprint).
3. Bảo vệ CAN/K, decoupling, pull-up CS, đèn nền, đầu nối.
4. Gán GPIO theo [`Pins.h`](../firmware/MotoECU/Pins.h) (đã khớp sơ đồ này).
