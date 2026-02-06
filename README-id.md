# FansElectronics_License

[🇺🇸 English Version](README.md)

---

## Deskripsi 📖
**FansElectronics License** adalah Arduino Library untuk membuat sistem lisensi offline pada perangkat berbasis **ESP32 dan ESP8266**.

Library ini dirancang untuk membantu developer dan produsen perangkat IoT agar dapat:
- Mengunci firmware ke hardware tertentu
- Mengaktifkan fitur berdasarkan lisensi
- Mencegah cloning perangkat
- Menjalankan verifikasi lisensi tanpa koneksi internet

Library mendukung 3 mode keamanan:

## Mode Kamanan 🔒

| Mode | Platform | Keamanan | Deskripsi | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐ | Hanya pengikatan perangkat (device binding) | ✅ Terimplementasi |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Tanda tangan kunci rahasia (secret-key) | ✅ Terimplementasi |
| ECDSA | ESP32 | ⭐⭐⭐⭐⭐ | Kriptografi kunci publik | ✅ Terimplementasi |
| AES | ESP32 & ESP8266 | ⭐⭐⭐⭐ | Berkas lisensi terenkripsi (kerahasiaan) | 🚧 Pembaruan Berikutnya |
| Ed25519 | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Tanda tangan kunci publik modern | 🚧 Pembaruan Berikutnya |

Cocok digunakan untuk:
- Produk IoT komersial
- Controller LED Matrix / Display
- Smart device offline 24/7
- Produk OEM / mass production

---

## Fitur Utama ✨
- Device ID unik berbasis hardware
- Offline license verification (tanpa internet)
- Multi encryption mode (LIGHT, HMAC, ECDSA)
- Support ESP32 & ESP8266
- License berbasis JSON (mudah dikustom)
- Tools generator license (CLI & Web)
- Sistem siap produksi (production-ready)

---

## Cara Kerja Singkat ❓
1️⃣ Device membuat **Device ID unik**  
2️⃣ Developer membuat file `license.json` menggunakan tool generator  
3️⃣ License disimpan di LittleFS perangkat  
4️⃣ Firmware memverifikasi lisensi saat boot  

Jika valid → fitur aktif  
Jika tidak valid → perangkat terkunci

---

## Struktur License File 🔑
Contoh `license.json`:

```json
{
  "data": {
    "device_id": "ABC123",
    "product": "PRODUCT NAME",
    "serial": "SN001",
    "expiry": 2026-02-06
  },
  "signature": "BASE64_SIGNATURE"
}
```
---
## Perbandingan Performa pada ESP (Estimasi) 📊

| Mode | ESP8266 RAM Usage | ESP32 RAM Usage | Verify Speed ESP8266 | Verify Speed ESP32 | License Size |
|---|---|---|---|---|---|
| LIGHT | ~1 KB | ~1 KB | < 1 ms | < 1 ms | Tiny |
| HMAC-SHA256 | ~4–6 KB | ~3–4 KB | ~5–10 ms | ~2–4 ms | Small |
| ECDSA-P256 | ~20–25 KB | ~12–16 KB | ~120–250 ms | ~20–40 ms | Medium |
| AES-256 (decrypt) | ~4–6 KB | ~2–3 KB (HW accel) | ~8–15 ms | ~1–2 ms | Encrypted |
| Ed25519 | ~10–14 KB | ~8–12 KB | ~40–80 ms | ~8–15 ms | Small |

---

## Dibuat oleh 💻
- Writer            : Irfan Indra Kurniawan, ST
- Organisasi        : Fans Electronics
- Website           : www.fanselectronics.com
- Email             : info@fanselectronics.com
> **Catatan dari pembuat:** Silahkan kembangkan dan gunakan untuk ibadah, pendidikan, pribadi, atau komersil, jika ada pengembangan lebih lanjut disarankan untuk membagikan agar ilmunya bermanfaat 😍

---

## Buat Beli Kopi ☕
Sedikit Kopi dan jika ingin berdonasi untuk project-project di fanselectronics.com, melalui:
- info@fanselectronics.com
- www.facebook.com/FansElectronicsCom
- www.instagram.com/fanselectronics
- www.saweria.co/fanselectronics

---

## Terimakasih Kepada 🤲
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- GitHub
- Contributor
- Everyone who gives me coffee