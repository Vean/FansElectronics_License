# FansElectronics_License

[🇮🇩 Bahasa Indonesia](README.md)

---

## Description
**FansElectronics License** is an Arduino library designed to implement an offline licensing system for **ESP32 and ESP8266** devices.

This library helps IoT developers and hardware manufacturers to:
- Lock firmware to specific hardware
- Enable features based on license
- Prevent device cloning
- Verify licenses without internet connection

Supported security modes:

| Mode | Platform | Security | Description |
|---|---|---|---|
| LIGHT | ESP32 & ESP8266 | ⭐ | Device binding only |
| HMAC | ESP32 & ESP8266 | ⭐⭐⭐ | Secret-key signature |
| ECDSA | ESP32 | ⭐⭐⭐⭐⭐ | Public-key cryptography |

Perfect for:
- Commercial IoT products
- LED controller / display systems
- 24/7 offline devices
- OEM / mass production hardware

---

## Features
- Unique hardware-based Device ID
- Offline license verification
- Multiple encryption modes (LIGHT, HMAC, ECDSA)
- Supports ESP32 & ESP8266
- JSON-based license format
- License generator tools (CLI & Web)
- Production-ready architecture

---

## How It Works

1️⃣ Device generates a unique **Device ID**  
2️⃣ Developer creates `license.json` using generator tool  
3️⃣ License is stored in LittleFS  
4️⃣ Firmware verifies license at boot  

Valid license → features enabled  
Invalid license → device locked

---

## License File Structure

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

## Created By
- Writer            : Irfan Indra Kurniawan, ST
- Organisasi        : Fans Electronics
- Website           : www.fanselectronics.com
- Email             : info@fanselectronics.com
> **Author note:** Feel free to develop and use this library for education, personal, religious, or commercial purposes. If you improve this project, please consider sharing your contribution so the knowledge can benefit others 😍

---

## Buy me a Coffe ☕
If you would like to support projects from fanselectronics.com:
- info@fanselectronics.com
- www.facebook.com/FansElectronicsCom
- www.instagram.com/fanselectronics
- saweria.co/fanselectronics

---

## Thanks To
- Allah Subhanahu Wa Ta'ala
- Arduino.cc
- Contributor
- Donatur

