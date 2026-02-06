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

Supported security modes:

## Security Modes 🔒

| Mode | Platform | Security | Description | Status |
|---|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐ | Device binding only | ✅ Implemented |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Secret-key signature | ✅ Implemented |
| ECDSA | ESP32 | ⭐⭐⭐⭐⭐ | Public-key cryptography | ✅ Implemented |
| AES | ESP32 & ESP8266 | ⭐⭐⭐⭐ | Encrypted license file (confidentiality) | 🚧 Next Update |
| Ed25519 | ESP32 & ESP8266 | ⭐⭐⭐⭐⭐ | Modern public-key signature | 🚧 Next Update |

Perfect for:
- Commercial IoT products
- LED controller / display systems
- 24/7 offline devices
- OEM / mass production hardware

---

## Features ✨
- Unique hardware-based Device ID
- Offline license verification
- Multiple encryption modes (LIGHT, HMAC, ECDSA)
- Supports ESP32 & ESP8266
- JSON-based license format
- License generator tools (CLI & Web)
- Production-ready architecture

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

Example `license.json`:

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
## ESP Performance Comparison (Estimated) 📊

| Mode | ESP8266 RAM Usage | ESP32 RAM Usage | Verify Speed ESP8266 | Verify Speed ESP32 | License Size |
|---|---|---|---|---|---|
| LIGHT | ~1 KB | ~1 KB | < 1 ms | < 1 ms | Tiny |
| HMAC-SHA256 | ~4–6 KB | ~3–4 KB | ~5–10 ms | ~2–4 ms | Small |
| ECDSA-P256 | Unsupport | ~12–16 KB | Unsupport | ~20–40 ms | Medium |
| AES-256 (decrypt) | ~4–6 KB | ~2–3 KB (HW accel) | ~8–15 ms | ~1–2 ms | Encrypted |
| Ed25519 | ~10–14 KB | ~8–12 KB | ~40–80 ms | ~8–15 ms | Small |

---

## Created By 💻
- Writer            : Irfan Indra Kurniawan, ST
- Organisasi        : Fans Electronics
- Website           : www.fanselectronics.com
- Email             : info@fanselectronics.com
> **Author note:** Feel free to develop and use this library for education, personal, religious, or commercial purposes. If you improve this project, please consider sharing your contribution so the knowledge can benefit others 😍

---

## Buy me a Coffee ☕
If you would like to support projects from fanselectronics.com:
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
