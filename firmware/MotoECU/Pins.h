// Pins.h - Sơ đồ chân đề xuất cho Moto ECU Master (ESP32-S3)
// Module khuyến nghị: ESP32-S3-WROOM-1-N16R2 (PSRAM quad) -> GPIO35/36/37 còn dùng được.
// Arduino IDE: Board "ESP32S3 Dev Module", USB CDC On Boot = Enabled, Flash 16MB.
// Nếu dùng module R8 (PSRAM octal) thì GPIO35/36/37 bị chiếm -> đổi I2C sang chân khác.
#pragma once

// ---------- TFT + SD + Touch (chung bus FSPI, chân IOMUX mặc định để chạy 40-80MHz) ----------
#define PIN_SPI_SCK   12
#define PIN_SPI_MOSI  11
#define PIN_SPI_MISO  13
#define PIN_TFT_CS    10
#define PIN_TFT_DC     9
#define PIN_TFT_RST    8
#define PIN_TFT_BL     7   // PWM -> MOSFET/NPN điều khiển đèn nền (không nối thẳng GPIO)
#define PIN_SD_CS     14   // tuỳ chọn: lưu dump ECU / log
#define PIN_TOUCH_CS  15   // tuỳ chọn (XPT2046)
#define PIN_TOUCH_IRQ 16   // tuỳ chọn

// ---------- CAN (TWAI) -> TJA1051T/3 ----------
#define PIN_CAN_TX    17
#define PIN_CAN_RX    18
#define PIN_CAN_STB   21   // TJA1051 pin S: LOW = normal, HIGH = silent

// ---------- K-line (UART1) -> L9637D qua dịch mức ----------
#define PIN_K_TX      38   // qua 74AHCT1G125 (3.3V -> 5V)
#define PIN_K_RX      39   // qua chia áp 10k/20k (5V -> 3.3V)
#define PIN_COMM_5V_EN 40  // bật boost 5V + load switch cho CAN / K-line

// ---------- Đo nguồn (ADC1 - không xung đột WiFi) ----------
#define PIN_VBAT_SENSE 4   // chia áp 100k/100k từ VBAT  (ADC1_CH3)
#define PIN_VCAR_SENSE 5   // chia áp 100k/22k  từ 12V xe (ADC1_CH4)

// ---------- Trạng thái mạch sạc BQ24074 (open-drain, kéo lên 3V3) ----------
#define PIN_CHG_N      1   // LOW = đang sạc, HIGH = đầy / không sạc
#define PIN_PGOOD_N    2   // LOW = có nguồn vào (USB hoặc xe)

// ---------- Nút bấm (kéo lên 10k, nút nối GND) ----------
#define PIN_BTN_BOOT   0   // BOOT: vừa là nút nạp code, vừa dùng làm nút phụ
#define PIN_BTN_UP     41
#define PIN_BTN_DOWN   42
#define PIN_BTN_OK     6

// ---------- I2C (tuỳ chọn: MAX17048 fuel gauge / IO expander) - chỉ khi module không có PSRAM octal ----------
#define PIN_I2C_SDA    35
#define PIN_I2C_SCL    36
// GPIO37: dự phòng

// ---------- Chân KHÔNG dùng ----------
// 3        : strap (chọn nguồn JTAG)            45, 46: strap (VDD_SPI, ROM log)
// 19, 20   : USB D- / D+ (Type-C nạp code)      43, 44: UART0 TX/RX (header debug)
// 26..32   : Flash/PSRAM                        47, 48: vùng VDD_SPI 1.8V trên một số module

// ---------- Hằng số đo nguồn ----------
#define VBAT_DIV_RATIO (2.0f)            // (100k+100k)/100k
#define VCAR_DIV_RATIO (122.0f / 22.0f)  // (100k+22k)/22k
