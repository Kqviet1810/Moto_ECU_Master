# Moto ECU Master

Thiết bị đọc/ghi ECU xe máy Honda & Yamaha: ESP32-S3-WROOM-1-N4R2 + TFT 2.8" + CAN (SN65HVD230) + K-line (L9637D), nguồn 12V xe hoặc pin Li-Po 3.7V 2000 mAh (sạc qua USB-C, BQ24074 power-path).

| Tài liệu | Nội dung |
|---|---|
| [docs/schematic/](docs/schematic/) | Sơ đồ nguyên lý Rev 1.0 (PDF) |
| [docs/01_danh_gia_so_do_rev1.0.md](docs/01_danh_gia_so_do_rev1.0.md) | Đánh giá sơ đồ thật: đúng / sai / thiếu, ưu tiên sửa trước khi vẽ PCB |
| [docs/02_mach_nguon_pin_lipo.md](docs/02_mach_nguon_pin_lipo.md) | Mạch sạc/xả, ổn áp, cách ly nguồn, 2 chân đo, thời gian dùng pin |
| [firmware/MotoECU/Pins.h](firmware/MotoECU/Pins.h) | Định nghĩa chân khớp sơ đồ (đánh dấu [SCHEMATIC] / [NEW]) |
| [firmware/MotoECU/PowerMonitor.h](firmware/MotoECU/PowerMonitor.h) | Đọc % pin, 12V xe, trạng thái sạc |
| [tools/battery_runtime.py](tools/battery_runtime.py) | Máy tính thời gian chạy pin |

Arduino IDE: Board *ESP32S3 Dev Module*, USB CDC On Boot = Enabled, Flash 4 MB, PSRAM = QSPI.
Firmware chưa biên dịch/thử trên phần cứng thật.
