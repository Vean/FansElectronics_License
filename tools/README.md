# 🛠️ FansElectronics License Generator

[🇮🇩 Bahasa Indonesia](README-ID.md)

---

# 🚀 FEL Generator (Windows Desktop App)

The primary license generator is the **FEL Generator.exe** desktop application, designed to run natively on Windows.

![FEL Generator](FEL_Generator.png)

## ✨ Features

- Simple and user-friendly GUI
- Portable (no installation required)
- No PHP or additional frameworks needed
- 100% Offline operation
- Generate, copy, and save `license.json` with a single click

## How to Use

1. Download **FEL Generator.exe** from the **Releases** page or the **tools** folder.
2. Run the application.
3. On the first launch, the application automatically creates:

```text
config.json
private_key.pem
public_key.pem
```

4. Replace the contents of `private_key.pem` with your **ECDSA Private Key** (or generate one first using `generate_keys.sh`).
5. Enter the **Device ID**.
6. Select the encryption mode.
7. Click **Generate License**.
8. Save the generated file as `license.json`.

## Application Configuration

Before generating licenses, make sure the default configuration matches your Arduino firmware, especially the **HMAC_SECRET_KEY** and **AES_SECRET_KEY**.

```json
{
  "HMAC_SECRET_KEY": "MY_HMAC_SECRET",
  "AES_SECRET_KEY": "MY_AES_SECRET",
  "DefaultEncryptionMode": "ECDSA_AES",
  "DefaultFileName": "license.json",
  "DefaultParameters": {
    "product_name": "FEL Generator",
    "product_version": "1.0.0",
    "product_copyright": "FansElectronics.com"
  }
}
```

> **Important:** The secret keys in `config.json` must be identical to those defined in your Arduino program.

---

# 💻 Alternative: PHP CLI

The original PHP version is still included for developers who want to integrate the license generator into a web dashboard or backend system.

## Requirements

Download PHP:

https://www.php.net/downloads.php

Verify the installation:

```bash
php -v
```

---

## 🔐 Step 1 — Generate Key Pair

Run the script from Command Prompt (Windows) or Terminal (Linux/macOS):

```bash
./generate_keys.sh
```

The following files will be created:

```text
private_key.pem
public_key.pem
```

> ⚠️ **Recommended:** Use a different key pair for every product. If one private key is compromised, your other products remain secure. Always store private keys safely.

---

## 🔑 Step 2 — Add the Public Key to Your Firmware

Copy the contents of `public_key.pem` into your ESP8266/ESP32 firmware.

```cpp
const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
PASTE_PUBLIC_KEY_HERE
-----END PUBLIC KEY-----
)KEY";
```

---

## 🧠 Step 3 — Get the Device ID

Upload your firmware to the ESP8266 or ESP32.

Open the **Serial Monitor** and copy the displayed Device ID.

Example:

```text
Device ID : ABCEFGHIJ1234567890
```

---

## 📄 Step 4 — Generate a License

### A. Desktop App (Recommended)

1. Open **FEL Generator.exe**
2. Paste the Device ID
3. Click **Generate**
4. Save the output as `license.json`

### B. PHP CLI

```bash
php generate_license.php encryption=ECDSA device_id=ABCEFGHIJ1234567890 product="LED Controller"
```

---

## 📦 Step 5 — Upload the License to ESP

Place the generated file in:

```text
/data/license.json
```

Upload it using **ESP LittleFS Upload**.

**Arduino IDE**

```text
Tools
 └── ESP LittleFS Upload
```

---

# 🧾 Example `license.json`

```json
{
  "data": {
    "device_id": "ABCEFGHIJ1234567890",
    "product": "LED Controller"
  },
  "signature": "BASE64_SIGNATURE"
}
```

---

# 🏭 Production Workflow

```text
ESP Device
    │
    │ 1. Displays Device ID
    ▼
Customer
    │
    │ 2. Sends Device ID
    ▼
Developer
    │
    │ 3. Generates license.json
    ▼
License File
    │
    │ 4. Uploads to LittleFS
    ▼
ESP Activated ✅
```

---

# 🚨 Security Warning

## NEVER SHARE

```text
private_key.pem
```

Your **Private Key** is the **Root of Trust**. Anyone with this file can generate valid licenses for your devices.

Only these files are safe to distribute:

- ✅ `public_key.pem`
- ✅ `license.json`

---

# 📜 License

Copyright © FansElectronics.com | Offline License System for ESP8266 & ESP32