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
- (NEW) Encrypt payload data to protect commercial secrets

Tested on Arduino **ESP32 Core 2.0.17** (Recommended), Wemos D1 Mini (ESP8266), ESP32 DevKit, ESP32S, ESP32-S3.

---

## What's New in v2.1.0 🚀

New update focused on **Payload Confidentiality (AES-256-CBC)** to fully encrypt the JSON payload.
<p align="justify">AES in this library <b>is not a standalone</b> signature mode. Instead, it acts as an encryption layer combined with our signature modes (Encrypt-then-MAC architecture). For example, combining AES with ECDSA creates FEL_MODE_ECDSA_AES.</p>
<small><b>Note:</b> If you want to run AES as a standalone encryption without any cryptographic signature validation, that is exactly what <b>FEL_MODE_LIGHT_AES</b> does!</small>

### Breaking change (one-line migration)`
```cpp
// v1.x
FansElectronics_License license(HMAC);

// v2.0.X
StaticJsonDocument<1024> doc;   // ESP8266
DynamicJsonDocument doc(1024);  // ESP32
FansElectronics_License license(doc, HMAC);

// v2.1.X
FansElectronics_License license(doc, FEL_MODE_HMAC);
LicenseStatus status = license.verifyLicense(
    HMAC_SECRET,     // Public Key (ECDSA) or HMAC Secret
    PRODUCT_SECRET,  // Device binding secret
    FEL_ID_LEN_64,   // ID Length
    true,            // Use Flash Size
    AES_SECRET       // (NEW) AES Decryption Key
);
```

### Why this change?
Previously the JSON memory size was fixed inside the library.  
Now **users control the memory size**, making the library stable for production projects.

Benefits:
- No need to edit library source anymore
- Safer for ESP8266 heap
- Scalable for large projects
- (NEW) AES-256-CBC payload encryption support
---

## Features ✨
- Unique hardware-based Device ID
- Offline license verification
- Multiple encryption modes (LIGHT, HMAC, ECDSA, AES)
- Supports ESP32 & ESP8266
- JSON-based license format
- License generator tools (CLI & Web)
- Production-ready architecture

---
## Security Modes 🔒

| Mode | Platform | Security | Description | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐😅 | Device binding only (No cryptography) | ✅ Implemented |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Secret-key signature (Authenticity) | ✅ Implemented |
| ECDSA | ESP32 | ⭐⭐⭐⭐ | Public-key signature (High Authenticity) | ✅ Implemented |
| LIGHT_AES | ESP32 & ESP8266 | ⭐⭐⭐ | Device binding + AES Encrypted payload only (Confidentiality)) | ✅ Implemented |
| HMHMAC_AESAC | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Encrypt-then-MAC (Confidentiality + Secret-key) | ✅ Implemented |
| ECDSA_AES | ESP32 | ⭐⭐⭐⭐⭐⭐ | Ultimate (Confidentiality + Public-key) | ✅ Implemented |
| ED25519 | ESP32 | ⭐⭐⭐⭐⭐ | Signature modern public key (ESP32 Core > 3.X) | 🚧 Next Update? |


Perfect for:
- Commercial IoT products
- LED controller / display systems
- 24/7 offline devices
- OEM / mass production hardware

---

## ArduinoJson Requirement ⚠️
This library uses **ArduinoJson v6.21.5**.

Why not ArduinoJson v7?
- ESP8266 requires deterministic memory allocation
- v7 uses dynamic heap allocation
- v6 fixed memory pool is more stable for long-running IoT devices
<small><b>Note:</b> If using <b>ESP32</b>, use ArduinoJson V7 no problem.</small>

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
