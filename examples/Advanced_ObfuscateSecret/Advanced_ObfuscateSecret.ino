/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-07
  Updated     : 2026-10-22
  Library     : FansElectronics_License v2.2.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Obfuscated_Secret (Full Obfuscation: Secret & Public Key)
============================================================ */

#include <ArduinoJson.h>
#include <FansElectronics_License.h>

/* ============================================================
   SHARED XOR KEY (32-bit Master Obfuscation Key)
============================================================ */
// Kunci 32-bit yang sama untuk semua secret/key yang diobfuscate
#define XOR_KEY 0x12345678UL

/* ============================================================
   1. OBFUSCATED PRODUCT SECRET ("MY_SECRET")
============================================================ */
// Original Plaintext: "MY_SECRET"
const uint8_t SECRET_OBF[] PROGMEM = {
    0x5F, 0x44, 0x6E, 0x48, 0x44, 0xBC, 0x6E, 0x7E, 0x56};

/* ============================================================
   2. OBFUSCATED ECDSA PUBLIC KEY (PEM Format)
============================================================ */
/*
   ORIGINAL PLAIN TEXT PUBLIC KEY (FOR REFERENCE):
   ------------------------------------------------------------
   -----BEGIN PUBLIC KEY-----
   MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEu2s2XllDI4fywu45IFhddil+7Egn
   hVwhQLYTEp7VjGE/OlCWzeVPrx8cQxl25Fw1497TsNyW3TP+QxwVXbnaZw==
   -----END PUBLIC KEY-----
   ------------------------------------------------------------
*/

// Hasil obfuscate format PEM ECDSA di atas dengan XOR_KEY 0x12345678UL
const uint8_t PUBLIC_KEY_OBF[] PROGMEM = {
    0x3F, 0x38, 0x1E, 0x07, 0x3F, 0x5F, 0x78, 0x75, 0x59, 0x54, 0x13, 0x62,
    0x61, 0x5F, 0x79, 0x47, 0x77, 0x41, 0x50, 0x73, 0x33, 0x2A, 0x0C, 0x15,
    0x33, 0x15, 0x51, 0x54, 0x63, 0x6D, 0x47, 0x6E, 0x71, 0x63, 0x6F, 0x4B,
    0x5D, 0x39, 0x5A, 0x39, 0x58, 0x7B, 0x35, 0x33, 0x55, 0x49, 0x53, 0x4F,
    0x5D, 0x5F, 0x4A, 0x6B, 0x58, 0x6F, 0x67, 0x5E, 0x31, 0x57, 0x55, 0x63,
    0x4F, 0x4B, 0x47, 0x74, 0x58, 0x7F, 0x56, 0x58, 0x61, 0x67, 0x53, 0x49,
    0x69, 0x7F, 0x5E, 0x4C, 0x4F, 0x7F, 0x4D, 0x41, 0x49, 0x5F, 0x64, 0x7B,
    0x31, 0x11, 0x51, 0x5A, 0x6A, 0x43, 0x72, 0x79, 0x54, 0x58, 0x71, 0x41,
    0x71, 0x68, 0x75, 0x63, 0x79, 0x67, 0x78, 0x4F, 0x77, 0x56, 0x40, 0x68,
    0x6F, 0x68, 0x48, 0x59, 0x72, 0x6B, 0x6F, 0x67, 0x41, 0x5B, 0x7B, 0x49,
    0x74, 0x68, 0x61, 0x7B, 0x51, 0x77, 0x73, 0x5A, 0x73, 0x49, 0x76, 0x5E,
    0x40, 0x48, 0x4C, 0x62, 0x4D, 0x73, 0x46, 0x58, 0x71, 0x63, 0x79, 0x5B,
    0x4D, 0x4B, 0x30, 0x2A, 0x21, 0x02, 0x3F, 0x38, 0x1E, 0x07, 0x3F, 0x59,
    0x53, 0x59, 0x30, 0x4F, 0x7B, 0x7A, 0x58, 0x56, 0x55, 0x3B, 0x53, 0x5D,
    0x6B, 0x4C, 0x33, 0x2A, 0x0C, 0x15, 0x33};

#define JSON_MEMORY 1024

#if defined(ESP8266)
StaticJsonDocument<JSON_MEMORY> doc;
#elif defined(ESP32)
DynamicJsonDocument doc(JSON_MEMORY);
#endif

FansElectronics_License license(doc, FEL_MODE_ECDSA);

void setup()
{
    Serial.begin(115200);
    Serial.println("\n=== FULL OBFUSCATED SECRET & PUBLIC KEY ===");

    // 1. Decode PRODUCT_SECRET ke RAM saat runtime
    String PRODUCT_SECRET = license.decodeSecret(
        SECRET_OBF,
        sizeof(SECRET_OBF),
        XOR_KEY);
    Serial.println("Secret      : " + PRODUCT_SECRET);

    // 2. Decode PUBLIC_KEY ke RAM saat runtime menggunakan XOR_KEY yang sama
    String PUBLIC_KEY = license.decodeSecret(
        PUBLIC_KEY_OBF,
        sizeof(PUBLIC_KEY_OBF),
        XOR_KEY);
    Serial.println("Public Key  : Decoded successfully (" + String(PUBLIC_KEY.length()) + " bytes)");
    Serial.println();

    auto info = license.getDeviceInfo();
    Serial.println("MODEL       : " + info.chipModel);
    Serial.println("MAC         : " + info.mac);
    Serial.println("FLASH SIZE  : " + info.flashSize);

    String deviceID = license.generateDeviceID(PRODUCT_SECRET);
    Serial.println("DeviceID    : " + deviceID);
    Serial.println();

    if (!license.loadLicense("/license.json"))
    {
        Serial.println("👀 License not found");
        return;
    }

    // Verifikasi lisensi menggunakan string hasil decode (.c_str())
    LicenseStatus status = license.verifyLicense(
        PUBLIC_KEY.c_str(),
        PRODUCT_SECRET);

    if (status == FEL_LICENSE_OK)
    {
        Serial.println("✅ License Valid");
    }
    else
    {
        Serial.println("❌ License Invalid");
    }

    license.printLicenseData(Serial);
}

void loop()
{
}