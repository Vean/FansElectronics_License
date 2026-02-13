# FansElectronics_License

[🇬🇧 English](README.md)

---

## Deskripsi 📖
**FansElectronics License** adalah library Arduino yang dirancang untuk membuat sistem lisensi **offline** pada perangkat **ESP32 dan ESP8266**.

Library ini membantu developer IoT dan produsen hardware untuk:
- Mengunci firmware ke hardware tertentu
- Mengaktifkan fitur berdasarkan lisensi
- Mencegah cloning perangkat
- Memverifikasi lisensi tanpa koneksi internet

---

## Perubahan di v2.0.0 🚀

Pembaruan besar berfokus pada **stabilitas memori dan kemudahan penggunaan**.

### Breaking change (perubahan 1 baris)
```cpp
// v1.x
FansElectronics_License license(HMAC);

// v2.0.0
StaticJsonDocument<1024> doc;   // ESP8266
DynamicJsonDocument doc(1024);  // ESP32
FansElectronics_License license(doc, HMAC);
```

### Kenapa perubahan ini?
Sebelumnya ukuran memori JSON dikunci di dalam library.  
Sekarang **user mengontrol alokasi memori sendiri**, sehingga library lebih stabil untuk project produksi.

Keuntungan:
- Tidak perlu mengedit source library lagi
- Lebih aman untuk heap ESP8266
- Lebih scalable untuk project besar

---

## Fitur ✨
- Device ID unik berbasis hardware
- Verifikasi lisensi offline
- Multi mode enkripsi (LIGHT, HMAC, ECDSA)
- Mendukung ESP32 & ESP8266
- Format lisensi berbasis JSON
- Tools generator lisensi (CLI & Web)
- Arsitektur siap produksi

---

## Mode Keamanan 🔒

| Mode | Platform | Keamanan | Deskripsi | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐ | Binding perangkat saja | ✅ Implemented |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Signature berbasis secret key | ✅ Implemented |
| ECDSA | ESP32 | ⭐⭐⭐⭐⭐ | Kriptografi public key | ✅ Implemented |
| AES | ESP32 & ESP8266 | ⭐⭐⭐⭐ | Enkripsi file lisensi (kerahasiaan) | 🚧 Next Update |
| Ed25519 | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Signature modern public key | 🚧 Next Update |

Sangat cocok untuk:
- Produk IoT komersial
- Sistem LED controller / display
- Perangkat offline 24/7
- Hardware OEM / produksi massal

---

## Kebutuhan ArduinoJson ⚠️
Library ini menggunakan **ArduinoJson v6.21.5**.

Kenapa bukan ArduinoJson v7?
- ESP8266 membutuhkan alokasi memori yang statik (ditentukan diawal)
- v7 menggunakan alokasi heap dinamis
- Memory pool tetap pada v6 lebih stabil untuk perangkat IoT jangka panjang

---

## Cara Kerja ❓

1️⃣ Perangkat membuat **Device ID unik**  
2️⃣ Developer membuat file `license.json` menggunakan generator  
3️⃣ Lisensi disimpan di LittleFS  
4️⃣ Firmware memverifikasi lisensi saat boot  

Lisensi valid → fitur aktif  
Lisensi tidak valid → perangkat terkunci

---

## Struktur File Lisensi 🔑

```json
{
  "data": {
    "device_id": "ABC123",
    "product": "PRODUCT NAME",
    "serial": "SN001",
    "panel": 64
  },
  "signature": "BASE64_SIGNATURE"
}
```

---

## Perbandingan Performa ESP (Estimasi) 📊

| Mode | RAM ESP8266 | RAM ESP32 | Kecepatan ESP8266 | Kecepatan ESP32 | Ukuran Lisensi |
|---|---|---|---|---|---|
| LIGHT | ~1 KB | ~1 KB | < 1 ms | < 1 ms | Sangat kecil |
| HMAC-SHA256 | ~4–6 KB | ~3–4 KB | ~5–10 ms | ~2–4 ms | Kecil |
| ECDSA-P256 | Tidak support | ~12–16 KB | Tidak support | ~20–40 ms | Sedang |
| AES-256 | ~4–6 KB | ~2–3 KB (HW accel) | ~8–15 ms | ~1–2 ms | Terenkripsi |
| Ed25519 | ~10–14 KB | ~8–12 KB | ~40–80 ms | ~8–15 ms | Kecil |

---

## Dibuat Oleh 💻
- Penulis           : Irfan Indra Kurniawan, ST
- Organisasi        : Fans Electronics
- Website           : www.fanselectronics.com
- Email             : info@fanselectronics.com

> **Catatan Penulis:** Silakan gunakan dan kembangkan library ini untuk pendidikan, pribadi, ibadah, maupun komersial.

---

## Buy me a Coffee ☕
- info@fanselectronics.com
- www.facebook.com/FansElectronicsCom
- www.instagram.com/fanselectronics
- www.saweria.co/fanselectronics

---

## Terima Kasih 🤲
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- GitHub
- Contributor
- Semua yang memberi saya kopi ☕