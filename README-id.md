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
- Mengenkripsi data payload untuk melindungi rahasia komersial (AES-256-CBC)
- Melindungi kunci rahasia dari dump memori flash dengan Obfuscation 32-bit
- Mencegah bypass memori runtime melalui Hardware Entropy Checksum 32-bit

Telah diuji pada Arduino **ESP32 Core 2.0.17** (Direkomendasikan), Wemos D1 Mini (ESP8266), ESP32 DevKit, ESP32S, ESP32-S3.

---

## Tools Web Resmi 🛠️
Untuk mempermudah pembuatan kunci dan pengamanan firmware, gunakan tools berbasis web resmi kami:
- 🛡️ **Secret Key Obfuscator:** [https://app.fanselectronics.com/tools/secret-key-obfuscation/](https://app.fanselectronics.com/tools/secret-key-obfuscation/)  
  *Mengubah kunci teks biasa (Product Secret, AES, HMAC, ECDSA) menjadi array byte PROGMEM 32-bit yang tersamar.*
- 🔑 **Secret Key Generator:** [https://app.fanselectronics.com/tools/secret-key-generator/](https://app.fanselectronics.com/tools/secret-key-generator/)  
  *Membuat pasangan kunci ECDSA NIST P-256, HMAC keys, AES keys, dan Product Secret.*
- 🔐 **[FEL_Generator.exe](https://github.com/Vean/FansElectronics_License/raw/refs/heads/V2.X.X/tools/FEL_Generator.exe)** *untuk generate file license.json. Aplikasi ini khusus Windows saja, dan membutuhkan .NET Core 6.x.*

---

## Apa yang Baru di v2.3.0 🚀

### 🛡️ Anti-Tamper Hardware Entropy & Validasi Tanpa Beban CPU
Pada versi sebelumnya, status verifikasi lisensi mengandalkan variabel boolean biasa di RAM yang rentan terhadap manipulasi memori (*memory patching* dari `0` menjadi `1`).

Pada **v2.3.0**, otorisasi diikat langsung ke identitas fisik silikon perangkat:
- **32-bit Hardware Entropy Checksum:** Validasi memori tidak lagi menggunakan flag boolean statis. Pustaka menghitung token 32-bit dinamis dari eFuse/MAC fisik pabrik dan Product Key. Menyuntikkan nilai `1` di RAM tidak akan membuka akses fitur.
- **Eksekusi `license.isValid()` Instan:** Tidak perlu lagi menjalankan fungsi kriptografi berat (`verifyLicense`) secara berkala dengan timer. `isValid()` dievaluasi dalam hitungan nanodetik (komparasi register integer CPU tunggal)—bebas kedipan (*zero flicker*) pada panel dot matrix dan audio tidak tersendat.
- **Optimasi Debug Guard:** Fungsi `printLicenseData()` dilindungi oleh makro `#define FEL_ENABLE_DEBUG true`. Saat mode debug dimatikan, seluruh kode cetak diagnostik otomatis dipangkas dari file biner untuk menghemat Flash dan RAM.

```cpp
#if DEBUGGING
  #define FEL_ENABLE_DEBUG true
#else
  #define FEL_ENABLE_DEBUG false
#endif

#include <FansElectronics_License.h>

// Pasang pengaman langsung di dalam loop utama atau fungsi inti:
void loop() {
  if (license.isValid()) {
    // Jalankan operasi normal (render panel LED, baca sensor, audio)
  } else {
    // Berikan gangguan halus atau kunci fitur ke mode demo
  }
}
```

---

## Apa yang Ada di v2.2.0 Sebelumnya 📦

### 🛡️ Firmware Hardening: Obfuscation 32-bit Rolling XOR
Menyimpan kunci rahasia (HMAC, AES, Product Secret, maupun Public Key ECDSA) dalam bentuk teks biasa sangat rentan terbaca saat memori Flash di-dump (`strings firmware.bin`).

Pada **v2.2.0**, kami menghadirkan fungsi terpadu `decodeSecret()` berbasis algoritma **32-bit Rolling XOR + Dynamic Index Salting**:
- Kunci tetap tersamar di Flash memory (`PROGMEM`).
- Kunci hanya direkonstruksi ke RAM sementara saat proses verifikasi berlangsung.
- Kebal terhadap analisis frekuensi byte dan serangan brute force 1-byte.

```cpp
#define XOR_KEY 0x12345678UL

// Array byte yang dibuat melalui Fans Electronics Obfuscator
const uint8_t SECRET_OBF[] PROGMEM = { 0x5F, 0x44, 0x6E, 0x48, 0x44, 0xBC, 0x6E, 0x7E, 0x56 };

// Decode saat runtime (hanya ada di RAM)
String PRODUCT_SECRET = license.decodeSecret(SECRET_OBF, sizeof(SECRET_OBF), XOR_KEY);
```

---

## Apa yang Ada di v2.1.1 Sebelumnya 📦
- **Kerahasiaan Payload (AES-256-CBC):** Enkripsi payload JSON menggunakan arsitektur Encrypt-then-MAC (`FEL_MODE_LIGHT_AES`, `FEL_MODE_HMAC_AES`, `FEL_MODE_ECDSA_AES`).
- Perbaikan sistem cache dan kestabilan pipeline verifikasi lisensi.

---

## Fitur ✨
- ID Perangkat unik berbasis perangkat keras
- Verifikasi lisensi secara offline
- Berbagai mode enkripsi (LIGHT, HMAC, ECDSA, AES)
- Pengerasan firmware melalui 32-bit secret obfuscation
- Integritas anti-tamper memori berbasis checksum silikon 32-bit
- Mendukung ESP32 dan ESP8266
- Format lisensi berbasis JSON
- Rangkaian tools web generator resmi
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
| ED25519 | ESP32 | ⭐⭐⭐⭐⭐ | Tanda tangan kunci publik modern (ESP32 Core > 3.X) | 🚧 Pembaruan Berikutnya |

---

## Persyaratan ArduinoJson ⚠️
Pustaka ini menggunakan **ArduinoJson v6.21.5**.

Mengapa bukan ArduinoJson v7?
- ESP8266 memerlukan alokasi memori yang deterministik
- v7 menggunakan alokasi *heap* dinamis
- *Fixed memory pool* pada v6 lebih stabil untuk perangkat IoT yang beroperasi dalam jangka waktu lama
<small><b>Catatan:</b> Jika menggunakan <b>ESP32</b>, ArduinoJson v7 dapat digunakan tanpa kendala.</small>

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

## Peringatan Keamanan & Batasan (Security Advisory) ⚠️
- **Proteksi Level Perangkat Lunak vs Perangkat Keras:** Pustaka ini menghadirkan proteksi terbaik di tingkat firmware/software untuk ESP32 dan ESP8266. Namun, proteksi software saja tidak dapat menjamin keamanan mutlak dari serangan fisik tingkat lanjut (*advanced hardware attacks*) seperti JTAG debugging langsung atau pembongkaran silikon chip.
- **Rekomendasi Produksi Komersial (ESP32):** Untuk produk komersial bernilai tinggi, sangat disarankan mengombinasikan pustaka ini dengan fitur keamanan perangkat keras bawaan ESP32:
  - **Flash Encryption:** Mencegah pembacaan dan penyalinan firmware mentah dari chip flash SPI.
  - **Secure Boot:** Memastikan hanya firmware resmi bertanda tangan digital yang dapat dieksekusi oleh chip.

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