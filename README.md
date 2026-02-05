# FansElectronics_License

[🇺🇸 English Version](README-en.md)

---

## Deskripsi
**FansElectronics License** adalah Arduino Library untuk membuat sistem lisensi offline pada perangkat berbasis **ESP32 dan ESP8266**.

Library ini dirancang untuk membantu developer dan produsen perangkat IoT agar dapat:
- Mengunci firmware ke hardware tertentu
- Mengaktifkan fitur berdasarkan lisensi
- Mencegah cloning perangkat
- Menjalankan verifikasi lisensi tanpa koneksi internet

Library mendukung 3 mode keamanan:

| Mode | Platform | Keamanan | Keterangan |
|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐ | Device binding saja |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Signature berbasis secret |
| ECDSA | ESP32 | ⭐⭐⭐⭐⭐ | Public-key cryptography |

Cocok digunakan untuk:
- Produk IoT komersial
- Controller LED Matrix / Display
- Smart device offline 24/7
- Produk OEM / mass production

---

## Fitur Utama
- Device ID unik berbasis hardware
- Offline license verification (tanpa internet)
- Multi encryption mode (LIGHT, HMAC, ECDSA)
- Support ESP32 & ESP8266
- License berbasis JSON (mudah dikustom)
- Tools generator license (CLI & Web)
- Sistem siap produksi (production-ready)

---

## Cara Kerja Singkat
1️⃣ Device membuat **Device ID unik**  
2️⃣ Developer membuat file `license.json` menggunakan tool generator  
3️⃣ License disimpan di LittleFS perangkat  
4️⃣ Firmware memverifikasi lisensi saat boot  

Jika valid → fitur aktif  
Jika tidak valid → perangkat terkunci

---

## Struktur License File
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

## Dibuat oleh
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

## Terimakasih kepada
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- Contributor
- Donatur