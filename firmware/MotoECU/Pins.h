// Pins.h - Sơ đồ chân Moto ECU Master, KHỚP schematic Rev 1.0 (2026-11-12)
// Module: ESP32-S3-WROOM-1-N4R2 (flash 4MB, PSRAM quad 2MB -> GPIO35..37, 47, 48 dùng được)
// Arduino IDE: Board "ESP32S3 Dev Module", USB CDC On Boot = Enabled, Flash 4MB, PSRAM = "QSPI PSRAM".
// Mục [SCHEMATIC] = đã có trên sơ đồ Rev 1.0. Mục [NEW] = cần thêm vào phần cứng (xem docs/).
#pragma once

// ---------- [SCHEMATIC] SPI chung: TFT 2.8" ILI9341 + Touch XPT2046 + Micro-SD (FSPI IOMUX) ----------
#define PIN_SPI_SCK    12
#define PIN_SPI_MOSI   11
#define PIN_SPI_MISO   13   // KHUYẾN NGHỊ: không nối SDO của TFT (pin 9), chỉ SD + Touch dùng MISO
#define PIN_TFT_CS     10
#define PIN_TFT_DC     16
#define PIN_TFT_RST    18
#define PIN_TOUCH_CS   14
#define PIN_TOUCH_IRQ   8
#define PIN_SD_CS       9

// ---------- [NEW] Đèn nền TFT (hiện LED pin 8 đang nối 3V3, luôn sáng) ----------
#define PIN_TFT_BL     15   // PWM; module có transistor BL thì nối trực tiếp, không thì qua MOSFET

// ---------- [SCHEMATIC] CAN (TWAI) -> SN65HVD230 (3.3V) ----------
#define PIN_CAN_TX      4
#define PIN_CAN_RX      5

// ---------- [SCHEMATIC] K-line (UART1) -> L9637D (VCC 3.3V) ----------
#define PIN_K_TX       17
#define PIN_K_RX       21

// ---------- [SCHEMATIC] I2C: AT24C02 (0x50), có thể thêm MAX17048 (0x36) ----------
#define PIN_I2C_SDA     6
#define PIN_I2C_SCL     7

// ---------- [SCHEMATIC] USB native + UART0 ----------
#define PIN_USB_DN     19
#define PIN_USB_DP     20
#define PIN_UART0_TX   43
#define PIN_UART0_RX   44

// ---------- [NEW] Nút BOOT (hiện IO0 chưa nối) ----------
#define PIN_BTN_BOOT    0   // nút -> GND

// ---------- [NEW] Đo nguồn (ADC1 - không xung đột WiFi) ----------
#define PIN_VBAT_SENSE  1   // ADC1_CH0, chia 100k/100k từ BAT
#define PIN_VCAR_SENSE  2   // ADC1_CH1, chia 100k/20k  từ 12V xe (sau F1, trước D5)

// ---------- [NEW] Trạng thái BQ24074 (open-drain, kéo lên 3V3 bằng 100k) ----------
#define PIN_CHG_N      38   // LOW = đang sạc; HIGH = đầy / không sạc
#define PIN_PGOOD_N    39   // LOW = có nguồn vào (USB hoặc 12V xe)

// ---------- [NEW, tuỳ chọn] Điều khiển ECU để đọc/ghi flash (xác nhận theo đời xe) ----------
#define PIN_ECU_PWR_EN 40   // P-MOS high-side cấp nguồn ECU
#define PIN_ECU_BOOT   41   // mở-collector tới chân BOOT/MODE của ECU
#define PIN_ECU_RST    42   // mở-collector tới chân RESET của ECU

// ---------- Dự phòng: 35, 36, 37, 47, 48 ----------
// ---------- KHÔNG dùng: 3, 45, 46 (strap) | 26..32 (flash/PSRAM) ----------

// ---------- Hằng số đo nguồn ----------
#define VBAT_DIV_RATIO (2.0f)            // (100k+100k)/100k
#define VCAR_DIV_RATIO (6.0f)            // (100k+20k)/20k
