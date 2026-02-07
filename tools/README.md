# FansElectronics_License

[🇮🇩 Bahasa Indonesia](README-ID.md)

---

# 🛠️ License Generator Tools (Offline License System)

**FansElectronics_License** provides tools to generate `license.json` files **offline** without needing a server.

This allows developers to:
- Lock firmware to hardware
- Enable paid features
- Prevent device cloning
- Run licensing without internet

---

# ❓ Why PHP?

We use **PHP CLI** because:

- Cross-platform (Windows/Linux/Mac)
- Matches existing backend stack
- Works offline
- Easy to integrate with web dashboards

More generators will be added in future.

---

# 📦 Requirements

Download PHP:
https://www.php.net/downloads.php

Check installation:

```bash
php -v
```

---

# 🔐 STEP 1 — Generate Key Pair

Run:

```bash
./generate_keys.sh
```

Output:

```
keys/
 ├── private_key.pem
 └── public_key.pem
```

### ⚠️ IMPORTANT

Each product MUST use different keys.

If one product key leaks → other products remain safe.

---

# 🔑 STEP 2 — Insert Public Key into Firmware

```cpp
const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
PASTE_PUBLIC_KEY_HERE
-----END PUBLIC KEY-----
)KEY";
```

---

# 🧠 STEP 3 — Get Device ID

Upload example → open Serial Monitor → copy DeviceID.

---

# 📄 STEP 4 — Generate License (Terminal)

```bash
php generate_license.php \
encryption=ECDSA \
device_id=ABC123 \
product="LED Controller"
```

Output → `license.json`

---

## 🖱️ Easy Method (.BAT)

Create `generate_license.bat`

```bat
php license_generator.php encryption=ECDSA device_id=ABC123
pause
```

Double click to run.

---

# 📦 STEP 5 — Upload to ESP

Place file in:

```
/data/license.json
```

Upload LittleFS:

```
Tools → ESP LittleFS Upload
```

📺 Tutorial:
https://youtube.com/watch?v=COMING_SOON

---

# 🧾 Example license.json

```json
{
  "data": {
    "device_id": "ABC123",
    "product": "LED Controller"
  },
  "signature": "BASE64_SIGNATURE"
}
```

---

# 🏭 Production Workflow

1️⃣ Device shows Device ID  
2️⃣ Customer sends Device ID  
3️⃣ Developer generates license  
4️⃣ Upload to device  

---

# 🚨 Security Warning

NEVER SHARE:
```
private_key.pem
```

This is the root of trust.

---

# 🎉 Done
Your device is ready for production.
