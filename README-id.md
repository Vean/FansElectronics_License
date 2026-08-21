# FansElectronics_License

[🇺🇸 English Version](README.md)

---

## Deskripsi 📖
**FansElectronics License** adalah pustaka (library) Arduino yang dirancang untuk mengimplementasikan sistem lisensi offline bagi perangkat **ESP32 dan ESP8266**.

Pustaka ini membantu pengembang IoT dan produsen perangkat keras untuk:
- Mengunci firmware pada perangkat keras tertentu
- Mengaktifkan fitur berdasarkan lisensi
- Mencegah kloning perangkat
- Memverifikasi lisensi tanpa koneksi internet
- (BARU) Mengenkripsi data payload untuk melindungi rahasia komersial

Telah diuji pada Arduino **ESP32 Core 2.0.17** (Direkomendasikan), Wemos D1 Mini (ESP8266), ESP32 DevKit, ESP32S, ESP32-S3.

---

## Apa yang Baru di v2.1.0 🚀

Pembaruan baru berfokus pada **Kerahasiaan Payload (AES-256-CBC)** untuk mengenkripsi payload JSON secara menyeluruh.
<p align="justify">AES dalam pustaka ini <b>bukanlah</b> mode tanda tangan (signature) yang berdiri sendiri. Sebaliknya, AES berfungsi sebagai lapisan enkripsi yang dikombinasikan dengan mode tanda tangan kami (arsitektur Encrypt-then-MAC). Sebagai contoh, menggabungkan AES dengan ECDSA akan menghasilkan FEL_MODE_ECDSA_AES.</p>
<small><b>Catatan:</b> Jika Anda ingin menjalankan AES sebagai enkripsi mandiri tanpa validasi tanda tangan kriptografis apa pun, itulah fungsi dari <b>FEL_MODE_LIGHT_AES</b>!</small>

### Perubahan yang memengaruhi kode lama (migrasi satu baris)`
```cpp
// v1.x
FansElectronics_License license(HMAC);

// v2.0.X
StaticJsonDocument<1024> doc; // ESP8266
DynamicJsonDocument doc(1024); // ESP32
FansElectronics_License license(doc, HMAC);

// v2.1.X
FansElectronics_License license(doc, FEL_MODE_HMAC);
LicenseStatus status = license.verifyLicense(
HMAC_SECRET,     // Kunci Publik (ECDSA) atau HMAC Secret
PRODUCT_SECRET,  // Secret untuk pengikatan perangkat (device binding)
FEL_ID_LEN_64,   // Panjang ID
true,            // Gunakan Ukuran Flash
AES_SECRET       // (BARU) Kunci Dekripsi AES
);
```

### Mengapa ada perubahan ini?
Sebelumnya, ukuran memori JSON ditetapkan secara statis di dalam pustaka (library).
Sekarang **pengguna dapat mengatur ukuran memori**, menjadikan pustaka ini stabil untuk proyek produksi.

Manfaat:
- Tidak perlu lagi mengubah kode sumber pustaka
- Lebih aman bagi heap ESP8266
- Dapat diskalakan untuk proyek besar
- (BARU) Dukungan enkripsi payload AES-256-CBC
---

## Fitur ✨
- ID Perangkat unik berbasis perangkat keras
- Verifikasi lisensi secara offline
- Berbagai mode enkripsi (LIGHT, HMAC, ECDSA, AES)
- Mendukung ESP32 dan ESP8266
- Format lisensi berbasis JSON
- Alat pembuat lisensi (CLI & Web)
- Arsitektur siap produksi

---
## Mode Keamanan 🔒

| Mode | Platform | Keamanan | Deskripsi | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐😅 | Hanya pengikatan perangkat (Tanpa kriptografi) | ✅ Diimplementasikan |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Tanda tangan kunci rahasia (Otentisitas) | ✅ Diimplementasikan |
| ECDSA | ESP32 | ⭐⭐⭐⭐ | Tanda tangan kunci publik (Otentisitas tinggi) | ✅ Diimplementasikan |
| LIGHT_AES | ESP32 & ESP8266 | ⭐⭐⭐ | Pengikatan perangkat + Payload terenkripsi AES saja (Kerahasiaan) | ✅ Diimplementasikan |
| HMAC_AES | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Encrypt-then-MAC (Kerahasiaan + Kunci rahasia) | ✅ Diimplementasikan |
| ECDSA_AES | ESP32 | ⭐⭐⭐⭐⭐⭐ | Tingkat tertinggi (Kerahasiaan + Kunci publik) | ✅ Diimplementasikan |
| ED25519 | ESP32 | ⭐⭐⭐⭐⭐ | Tanda tangan kunci publik modern (ESP32 Core > 3.X) | 🚧 Pembaruan Berikutnya? |


Cocok untuk:
- Produk IoT komersial
- Pengontrol LED / sistem tampilan
- Perangkat offline yang beroperasi 24/7
- Perangkat keras OEM / produksi massal

---

## Persyaratan ArduinoJson ⚠️
Pustaka ini menggunakan **ArduinoJson v6.21.5**.

Mengapa bukan ArduinoJson v7?
- ESP8266 memerlukan alokasi memori yang deterministik
- v7 menggunakan alokasi *heap* dinamis
- *Fixed memory pool* pada v6 lebih stabil untuk perangkat IoT yang beroperasi dalam jangka waktu lama
<small><b>Catatan:</b> Jika menggunakan <b>ESP32</b>, tidak masalah menggunakan ArduinoJson V7.</small>

---

## Cara Kerja ❓

1️⃣ Perangkat menghasilkan **Device ID** yang unik  
2️⃣ Pengembang membuat `license.json` menggunakan alat generator  
3️⃣ Lisensi disimpan di LittleFS  
4️⃣ Firmware memverifikasi lisensi saat *booting*

Lisensi valid → fitur diaktifkan
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

## Dibuat Oleh 💻
- Penulis           : Irfan Indra Kurniawan, ST
- Organisasi        : Fans Electronics
- Situs Web         : www.fanselectronics.com
- Email             : info@fanselectronics.com

> **Catatan penulis:** Silakan kembangkan dan gunakan pustaka ini untuk tujuan pendidikan, pribadi, keagamaan, maupun komersial.

---

## Traktir Kopi ☕
- info@fanselectronics.com
- www.facebook.com/FansElectronicsCom
- www.instagram.com/fanselectronics
- www.saweria.co/fanselectronics

---

## Terima Kasih Kepada 🤲
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- GitHub
- Kontributor
- Semua orang yang mentraktir saya kopi