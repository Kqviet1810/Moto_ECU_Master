#!/usr/bin/env python3
"""Ước tính thời gian chạy pin Li-Po cho Moto ECU Master.
Sửa các số trong LOADS / hiệu suất rồi chạy:  python3 tools/battery_runtime.py
"""
VBAT_NOM = 3.7          # V, điện áp danh định Li-Po
CAP_MAH  = 2000         # mAh ghi trên pin
DERATE   = 0.85         # hệ số thực tế (cắt ở ~3.2V, lão hoá, nhiệt độ)
EFF_3V3  = 0.90         # buck-boost 3.3V (TPS63001/TPS63802) tại tải này
EFF_5V   = 0.88         # boost 5V (TPS61023)
IQ_MA    = 0.15         # mA dòng nghỉ của BQ24074 + converter + chia áp (phía pin)

# (mA @3.3V, mA @5V)
LOADS = {
    "Chạy chẩn đoán (TFT sáng 70%, CAN/K-line hoạt động, SD ghi log)": (70 + 15 + 70 + 8 + 5, 20),
    "Chờ / xem dữ liệu (TFT sáng 20%, ESP 80MHz)":                      (40 + 15 + 20 + 3,     0),
    "Truyền WiFi dump (TFT 50%, WiFi bật)":                             (130 + 15 + 50 + 8 + 5, 0),
    "Deep standby (màn tắt, ESP light-sleep)":                          (3,                    0),
}

def run():
    cap = CAP_MAH * DERATE
    print(f"Pin {CAP_MAH} mAh x {DERATE:.2f} = {cap:.0f} mAh dùng được ({cap*VBAT_NOM/1000:.2f} Wh)\n")
    for name, (i33, i5) in LOADS.items():
        p_load = i33 * 3.3 / EFF_3V3 + i5 * 5.0 / EFF_5V        # mW lấy từ VSYS
        i_bat = p_load / VBAT_NOM + IQ_MA                          # mA lấy từ pin
        h = cap / i_bat
        print(f"{name}\n   tải: {i33} mA@3V3 + {i5} mA@5V -> pin {i_bat:.0f} mA -> {h:.1f} h\n")
    # sạc
    for ichg in (0.5, 1.0):
        t = (CAP_MAH/1000*0.8)/ichg + 1.0   # ~80% dung lượng ở CC + ~1h pha CV
        print(f"Sạc ISET={ichg:.1f} A: ~{t:.1f} h (CC ~80% + CV, ước tính)")

if __name__ == "__main__":
    run()
