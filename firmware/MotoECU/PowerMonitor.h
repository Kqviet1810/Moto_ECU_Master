// PowerMonitor.h - đọc điện áp pin / nguồn xe + trạng thái sạc, hiển thị lên TFT
// Header-only, dùng với Pins.h. Chưa biên dịch thử trên phần cứng thật - cần hiệu chỉnh ADC.
#pragma once
#include <Arduino.h>
#include "Pins.h"

struct PowerStatus {
  float vbat;        // V
  float vcar;        // V (0 nếu không cắm xe)
  int   percent;     // 0..100, ước tính theo điện áp
  bool  usbOrCar;    // có nguồn vào mạch sạc (PGOOD)
  bool  charging;    // đang sạc
  bool  full;        // có nguồn vào và đã ngừng sạc
  bool  carPresent;  // vcar > 9V
};

class PowerMonitor {
 public:
  void begin() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    pinMode(PIN_CHG_N, INPUT);      // pull-up ngoài 100k lên 3V3
    pinMode(PIN_PGOOD_N, INPUT);
  }

  PowerStatus read() {
    PowerStatus s;
    s.vbat = readMilliVolts(PIN_VBAT_SENSE) * VBAT_DIV_RATIO / 1000.0f;
    s.vcar = readMilliVolts(PIN_VCAR_SENSE) * VCAR_DIV_RATIO / 1000.0f;
    s.carPresent = s.vcar > 9.0f;
    s.usbOrCar = (digitalRead(PIN_PGOOD_N) == LOW);
    s.charging = s.usbOrCar && (digitalRead(PIN_CHG_N) == LOW);
    s.full = s.usbOrCar && !s.charging;
    s.percent = percentFromVoltage(s.vbat, s.charging);
    return s;
  }

 private:
  // Trung bình 16 mẫu; analogReadMilliVolts dùng hiệu chuẩn eFuse của chip.
  static float readMilliVolts(int pin) {
    uint32_t acc = 0;
    for (int i = 0; i < 16; i++) acc += analogReadMilliVolts(pin);
    return acc / 16.0f;
  }

  // Đường cong xả Li-Po xấp xỉ (không tải nhẹ). Khi đang sạc điện áp bị đẩy lên
  // nên % chỉ mang tính tham khảo -> trừ 0.1V cho hợp lý.
  static int percentFromVoltage(float v, bool charging) {
    if (charging) v -= 0.10f;
    static const float lut[][2] = {
      {3.30f, 0},  {3.50f, 5},  {3.60f, 15}, {3.70f, 35}, {3.75f, 50},
      {3.80f, 60}, {3.90f, 75}, {4.00f, 87}, {4.10f, 95}, {4.20f, 100}};
    const int n = sizeof(lut) / sizeof(lut[0]);
    if (v <= lut[0][0]) return 0;
    if (v >= lut[n - 1][0]) return 100;
    for (int i = 1; i < n; i++) {
      if (v <= lut[i][0]) {
        float t = (v - lut[i - 1][0]) / (lut[i][0] - lut[i - 1][0]);
        return (int)(lut[i - 1][1] + t * (lut[i][1] - lut[i - 1][1]) + 0.5f);
      }
    }
    return 100;
  }
};
