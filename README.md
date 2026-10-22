# FansElectronics_License

[🇮🇩 Bahasa Indonesia](README-id.md)

---

## Description 📖
**FansElectronics License** is an Arduino library designed to implement an offline licensing system for **ESP32 and ESP8266** devices.

This library helps IoT developers and hardware manufacturers to:
- Lock firmware to specific hardware
- Enable features based on license
- Prevent device cloning
- Verify licenses without internet connection
- Encrypt payload data to protect commercial secrets (AES-256-CBC)
- Protect plain-text keys from flash dumps with 32-bit Obfuscation

Tested on Arduino **ESP32 Core 2.0.17** (Recommended), Wemos D1 Mini (ESP8266), ESP32 DevKit, ESP32S, ESP32-S3.

---

## Official Web Tools 🛠️
To simplify key generation and firmware hardening, use our official online web tools:
- 🛡️ **Secret Key Obfuscator:** [https://app.fanselectronics.com/tools/secret-key-obfuscation/](https://app.fanselectronics.com/tools/secret-key-obfuscation/)  
  *Converts plain-text keys (Product Secret, AES, HMAC, ECDSA) into hardened 32-bit PROGMEM byte arrays.*
- 🔑 **Secret Key Generator:** [https://app.fanselectronics.com/tools/secret-key-generator/](https://app.fanselectronics.com/tools/secret-key-generator/)  
  *Generates ECDSA NIST P-256 keypairs, high-entropy HMAC keys, AES keys, and Product Secrets.*
-  🔐 **[FEL_Generator.exe](https://github.com/Vean/FansElectronics_License/raw/refs/heads/V2.X.X/tools/FEL_Generator.exe)** *Used to generate the license.json file. This application is Windows-only and requires .NET Core 6.x.*
---

## What's New in v2.2.0 🚀

### 🛡️ Firmware Hardening: 32-bit Multi-Byte Rolling XOR Obfuscation
Storing secrets (HMAC keys, AES keys, Product Secret, or ECDSA Public Keys) as plain strings makes firmware vulnerable to simple flash memory dumps (`strings firmware.bin`).

In **v2.2.0**, we introduce a single unified `decodeSecret()` powered by **32-bit Rolling XOR + Dynamic Index Salting**:
- Keys remain scrambled in Flash memory (`PROGMEM`).
- Keys are decoded dynamically into RAM only during verification and cleared when done.
- Immune to single-byte frequency analysis and brute-force attacks.

```cpp
#define XOR_KEY 0x12345678UL

// Byte array generated via Fans Electronics Obfuscator
const uint8_t SECRET_OBF[] PROGMEM = { 0x5F, 0x44, 0x6E, 0x48, 0x44, 0xBC, 0x6E, 0x7E, 0x56 };

// Decode at runtime (RAM-only)
String PRODUCT_SECRET = license.decodeSecret(SECRET_OBF, sizeof(SECRET_OBF), XOR_KEY);
```

---

## What Was New in v2.1.1 📦
- **Payload Confidentiality (AES-256-CBC):** Full JSON payload encryption using Encrypt-then-MAC architecture (`FEL_MODE_LIGHT_AES`, `FEL_MODE_HMAC_AES`, `FEL_MODE_ECDSA_AES`).
- Cache reset and verification pipeline stability improvements.

---

## Features ✨
- Unique hardware-based Device ID
- Offline license verification
- Multiple encryption modes (LIGHT, HMAC, ECDSA, AES)
- Firmware hardening via 32-bit secret obfuscation
- Supports ESP32 & ESP8266
- JSON-based license format
- Dedicated web tools suite
- Production-ready architecture

---

## Security Modes 🔒

| Mode | Platform | Security | Description | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐😅 | Device binding only (No cryptography) | ✅ Implemented |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Secret-key signature (Authenticity) | ✅ Implemented |
| ECDSA | ESP32 | ⭐⭐⭐⭐ | Public-key signature (High Authenticity) | ✅ Implemented |
| LIGHT_AES | ESP32 & ESP8266 | ⭐⭐⭐ | Device binding + AES Encrypted payload only (Confidentiality) | ✅ Implemented |
| HMAC_AES | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Encrypt-then-MAC (Confidentiality + Secret-key) | ✅ Implemented |
| ECDSA_AES | ESP32 | ⭐⭐⭐⭐⭐⭐ | Ultimate (Confidentiality + Public-key) | ✅ Implemented |
| ED25519 | ESP32 | ⭐⭐⭐⭐⭐ | Signature modern public key (ESP32 Core > 3.X) | 🚧 Next Update |

---

## ArduinoJson Requirement ⚠️
This library uses **ArduinoJson v6.21.5**.

Why not ArduinoJson v7?
- ESP8266 requires deterministic memory allocation
- v7 uses dynamic heap allocation
- v6 fixed memory pool is more stable for long-running IoT devices
<small><b>Note:</b> If using <b>ESP32</b>, ArduinoJson v7 is supported without issue.</small>

---

## How It Works ❓

1️⃣ Device generates a unique **Device ID**  
2️⃣ Developer creates `license.json` using generator tool  
3️⃣ License is stored in LittleFS  
4️⃣ Firmware verifies license at boot  

Valid license → features enabled  
Invalid license → device locked

---

## License File Structure 🔑

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

## Created By 💻
- Writer            : Irfan Indra Kurniawan, ST
- Organization      : Fans Electronics
- Website           : www.fanselectronics.com
- Email             : info@fanselectronics.com

> **Author note:** Feel free to develop and use this library for education, personal, religious, or commercial purposes.

---

## Buy me a Coffee ☕
- info@fanselectronics.com
- www.facebook.com/FansElectronicsCom
- www.instagram.com/fanselectronics
- www.saweria.co/fanselectronics

---

## Thanks To 🤲
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- GitHub
- Contributor
- Everyone who gives me coffee