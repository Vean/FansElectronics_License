# FansElectronics_License

[🇺🇸 English Version](README.md)

---

# 🛠️ License Generator Tools (Offline License System)

Library **FansElectronics_License** menyediakan tools lengkap untuk membuat file `license.json` secara **offline** tanpa perlu server.

Dengan tools ini kamu dapat:
- Mengunci firmware ke hardware tertentu
- Mengaktifkan fitur berdasarkan lisensi
- Mencegah cloning perangkat
- Menjalankan sistem lisensi tanpa internet

---

# ❓ Kenapa Menggunakan PHP?

Generator lisensi dibuat menggunakan **PHP CLI** karena:

- Mudah digunakan di Windows, Linux, Mac
- Sama dengan stack backend FansElectronics
- Bisa dijalankan offline tanpa server
- Mudah diintegrasikan ke aplikasi web / dashboard

Kedepannya generator akan tersedia dalam bahasa lain.

---

# 📦 Prasyarat

Download PHP:
https://www.php.net/downloads.php

Cek instalasi:

```bash
php -v
```

---

# 🔐 STEP 1 — Generate Key Pair (Sekali saja)

Jalankan:

```bash
./generate_keys.sh
```

Hasil:

```
keys/
 ├── private_key.pem
 └── public_key.pem
```

### ⚠️ PENTING (WAJIB DIBACA)

Setiap **produk** HARUS memiliki key pair berbeda.

Contoh:
- Produk LED Controller → key A
- Produk Smart Fan → key B
- Produk Running Text → key C

Jika satu key bocor → produk lain tetap aman.

---

# 🔑 STEP 2 — Masukkan Public Key ke Firmware

```cpp
const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
ISI_PUBLIC_KEY_DISINI
-----END PUBLIC KEY-----
)KEY";
```

---

# 🧠 STEP 3 — Ambil Device ID dari ESP

Upload contoh sketch → buka Serial Monitor → copy DeviceID.

---

# 📄 STEP 4 — Generate License (CMD / Terminal)

Buka CMD / Terminal di folder tools.

Contoh:

```bash
php generate_license.php ^
encryption=ECDSA ^
device_id=ABC123 ^
product="LED Controller" ^
serial=SN001 ^
panel=10
```

Output → `license.json`

---

## 🖱️ Cara Mudah (Double Click .BAT)

Buat file `generate_license.bat`

Isi:

```bat
php generate_license.php encryption=ECDSA device_id=ABC123 product="LED Controller"
pause
```

User cukup double click.

---

# 📦 STEP 5 — Upload License ke ESP

Simpan file ke:

```
/data/license.json
```

Upload LittleFS:
```
Tools → ESP LittleFS Upload
```

📺 Tutorial video:
https://youtube.com/watch?v=COMING_SOON

---

# 🧾 Contoh license.json

```json
{
  "data": {
    "device_id": "ABC123",
    "product": "LED Controller",
    "serial": "SN001"
  },
  "signature": "BASE64_SIGNATURE"
}
```

---

# 🏭 Workflow Produksi

1️⃣ ESP tampilkan Device ID  
2️⃣ Customer kirim ke developer  
3️⃣ Generate license.json  
4️⃣ Upload ke perangkat  

---

# 🚨 Security Warning

JANGAN BAGIKAN:
```
private_key.pem
```

Ini adalah ROOT OF TRUST sistem lisensi.

---

# 🎉 Selesai
Perangkat siap produksi.
