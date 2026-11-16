// PowerMonitor.h - đọc điện áp pin / nguồn xe + trạng thái sạc (BQ24074) để hiển thị lên TFT.
// Header-only, dùng với Pins.h. CHƯA biên dịch/thử trên phần cứng thật - cần hiệu chỉnh ADC thực tế.
#pragma once
#include <Arduino.h>
#include "Pins.h"

enum PowerSource { SRC_BATTERY, SRC_CHARGING, SRC_FULL };

struct PowerStatus {
  float vbat;         // V
  float vcar;         // V (≈0 nếu không cắm xe)
  int   percent;      // 0..100, ước tính theo điện áp (đo khi sạc chỉ tham khảo)
  bool  inputPresent; // có nguồn vào mạch sạc (PGOOD): USB hoặc 12V xe
  bool  charging;
  bool  full;         // có nguồn vào và đã ngừng sạc
  bool  carPresent;   // vcar > 9V
  bool  lowBattery;   // <= 3.40V khi chạy pin
  bool  shutdownNow;  // <= 3.20V khi chạy pin -> nên deep-sleep
  PowerSource source;
};

class PowerMonitor {
 public:
  void begin() {
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_VBAT_SENSE, ADC_11db);
    analogSetPinAttenuation(PIN_VCAR_SENSE, ADC_11db);
    pinMode(PIN_CHG_N, INPUT);     // đã có pull-up 100k ngoài
    pinMode(PIN_PGOOD_N, INPUT);
  }

  PowerStatus read() {
    PowerStatus s;
    s.vbat = readMilliVolts(PIN_VBAT_SENSE) * VBAT_DIV_RATIO / 1000.0f;
    s.vcar = readMilliVolts(PIN_VCAR_SENSE) * VCAR_DIV_RATIO / 1000.0f;
    s.carPresent   = s.vcar > 9.0f;
    s.inputPresent = (digitalRead(PIN_PGOOD_N) == LOW);
    s.charging     = s.inputPresent && (digitalRead(PIN_CHG_N) == LOW);
    s.full         = s.inputPresent && !s.charging;
    s.percent      = percentFromVoltage(s.vbat, s.charging);
    bool onBat     = !s.inputPresent;
    s.lowBattery   = onBat && s.vbat <= 3.40f;
    s.shutdownNow  = onBat && s.vbat <= 3.20f;
    s.source = s.charging ? SRC_CHARGING : (s.full ? SRC_FULL : SRC_BATTERY);
    return s;
  }

 private:
  // Trung bình 16 mẫu; analogReadMilliVolts dùng hiệu chuẩn eFuse của chip.
  static float readMilliVolts(int pin) {
    uint32_t acc = 0;
    for (int i = 0; i < 16; i++) acc += analogReadMilliVolts(pin);
    return acc / 16.0f;
  }

  // Đường cong xả Li-Po xấp xỉ. Khi đang sạc điện áp bị đẩy lên -> trừ 0.1V.
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
