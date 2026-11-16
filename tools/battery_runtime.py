#!/usr/bin/env python3
"""Ước tính thời gian chạy pin Li-Po cho Moto ECU Master (kiến trúc: BQ24074 -> TPS63001 3.3V).
Sửa các số trong LOADS / hiệu suất rồi chạy:  python3 tools/battery_runtime.py
Chỉ chế độ CHẠY PIN (không có 12V). Tải đều ở rail 3.3V (CAN HVD230 + L9637D VCC cũng 3.3V).
"""
VBAT_NOM = 3.7          # V, điện áp danh định Li-Po
CAP_MAH  = 2000         # mAh ghi trên pin
DERATE   = 0.85         # hệ số thực tế (cắt ~3.2V, lão hoá, nhiệt độ)
EFF_3V3  = 0.90         # TPS63001 buck-boost tại tải này
EFF_PATH = 0.98         # FET BAT->OUT của BQ24074 (~50-100 mV rơi áp)
IQ_MA    = 0.15         # mA dòng nghỉ phía pin (BQ24074 + TPS63001 + chia áp 21 uA)

# mA @3.3V = ESP32 + TFT(+đèn nền) + CAN + SD + linh tinh
LOADS = {
    "Chẩn đoán CAN (ESP 60, TFT 70%=70, CAN 15, SD 8, khác 5)": 60 + 70 + 15 + 8 + 5,
    "Chờ / xem dữ liệu (ESP 40, TFT 20%=20, CAN 15, khác 3)":    40 + 20 + 15 + 3,
    "Truyền WiFi dump (ESP 130, TFT 50%=50, CAN 15, SD 8, khác 5)": 130 + 50 + 15 + 8 + 5,
    "Đèn nền 100% + ESP 60 + CAN 15 (xấu nhất khi chẩn đoán)":   60 + 100 + 15 + 5,
    "Màn tắt, ESP light-sleep":                                    3,
}

def run():
    cap = CAP_MAH * DERATE
    print(f"Pin {CAP_MAH} mAh x {DERATE:.2f} = {cap:.0f} mAh dùng được ({cap*VBAT_NOM/1000:.2f} Wh)\n")
    for name, i33 in LOADS.items():
        p_load = i33 * 3.3 / EFF_3V3 / EFF_PATH      # mW lấy từ pin
        i_bat = p_load / VBAT_NOM + IQ_MA             # mA lấy từ pin
        h = cap / i_bat
        t = f"{h:.1f} h" if h < 48 else f"{h/24:.0f} ngày"
        print(f"{name}\n   tải {i33} mA@3V3 -> pin {i_bat:.0f} mA -> {t}\n")
    # Sạc (BQ24074): ISET=0.5A. ~80% dung lượng ở CC + ~1h pha CV
    for ichg in (0.5, 0.9):
        tt = (CAP_MAH / 1000 * 0.8) / ichg + 1.0
        print(f"Sạc ISET={ichg:.1f} A: ~{tt:.1f} h (CC ~80% + CV, ước tính; tải hệ thống làm chậm thêm)")
    # Công suất lấy từ 12V khi vừa chạy vừa sạc (LM2596 80%, BQ24074 linear, TPS63001 90%)
    i33 = LOADS["Chẩn đoán CAN (ESP 60, TFT 70%=70, CAN 15, SD 8, khác 5)"]
    p5 = (i33 * 3.3 / EFF_3V3 + 0.5 * 4.2) / 0.88    # mW tại 5V_IN
    p12 = p5 / 0.82
    print(f"\n12V xe, chẩn đoán + sạc 0.5A: ~{p12:.0f} mW -> ~{p12/12:.0f} mA @12V (cầu chì 1A là dư)")

if __name__ == "__main__":
    run()
