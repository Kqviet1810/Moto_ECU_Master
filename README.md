# Moto ECU Master

Thiết bị đọc/ghi ECU xe máy Honda & Yamaha: ESP32-S3 + TFT + CAN (TJA1051T/3) + K-line (L9637D), nguồn 12V xe hoặc pin Li-Po 3.7V 2000 mAh (sạc qua USB-C).

| Tài liệu | Nội dung |
|---|---|
| [docs/01_danh_gia_pinout_va_checklist_pcb.md](docs/01_danh_gia_pinout_va_checklist_pcb.md) | Pinout đề xuất, thiếu/sai cần sửa trước khi vẽ PCB |
| [docs/02_mach_nguon_pin_lipo.md](docs/02_mach_nguon_pin_lipo.md) | Mạch sạc/xả, ổn áp, cách ly nguồn, thời gian dùng pin |
| [firmware/MotoECU/Pins.h](firmware/MotoECU/Pins.h) | Định nghĩa chân cho Arduino IDE |
| [firmware/MotoECU/PowerMonitor.h](firmware/MotoECU/PowerMonitor.h) | Đọc % pin, 12V xe, trạng thái sạc |
| [tools/battery_runtime.py](tools/battery_runtime.py) | Máy tính thời gian chạy pin |

Arduino IDE: Board *ESP32S3 Dev Module*, USB CDC On Boot = Enabled, Flash 16 MB.
