/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-07
  Updated     : 2026-02-13
  Library     : FansElectronics_License v2.0.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Basic_HMAC

  Copyright (c) 2016-2026 Fans Electronics

  ============================================================
  EN: WHY THIS EXAMPLE EXISTS
  This example demonstrates how to use "Feature Flags"
  in the license to enable/ disable certain features
  in your firmware. If a user attempts to modify the
  license to access paid features, the license verification
  will fail, making those features more secure for
  your commercial products.

  ID: MENGAPA CONTOH INI ADA
  Contoh ini menunjukkan cara menggunakan fitur
  "Feature Flags" pada lisensi untuk mengaktifkan/ menonaktifkan
  fitur tertentu pada firmware Anda. Apabila user mencoba untuk
  memodifikasi lisensi agar dapat mengakses fitur berbayar,
  verifikasi lisensi akan gagal, sehingga fitur tersebut lebih
  aman untuk produk komersial Anda.
============================================================ */

// EN: Include FansElectronics_License library & ArduinoJSON
// ID: Masukkan library FansElectronics_License & ArduinoJSON
#include <ArduinoJson.h>
#include <FansElectronics_License.h>

// EN: Define your product secret
// ID: Definisikan product secret Anda
#define PRODUCT_SECRET "MY_SECRET"

// EN: Define your HMAC secret
// ID: Definisikan HMAC secret Anda
#define HMAC_SECRET "MY_HMAC_SECRET"

// EN: Recomendation minimum memory, u can change it
// ID: Rekomendasi minimal memory, anda bisa memnggantinya
#define JSON_MEMORY 1024

// EN: Set ESP8266 & ESP32 JSON Memory Type
// ID: Set  ESP8266 & ESP32 JSON Tipe Memory
#if defined(ESP8266)
StaticJsonDocument<JSON_MEMORY> doc;
#elif defined(ESP32)
DynamicJsonDocument doc(JSON_MEMORY);
#endif

// EN: Create FansElectronics_License object in HMAC mode
// ID: Buat objek FansElectronics_License dalam mode HMAC
FansElectronics_License license(doc, HMAC); // JSON doc, Encryption Type

// ===== SIMULASI HARDWARE =====
int installedPanels = 4; // jumlah panel fisik terpasang
int userBrightness = 90; // brightness yang diminta user

// ------------------------------------------------------------
// HELPER: READ FEATURE FLAGS
// ------------------------------------------------------------
int getLicensedPanelLimit()
{
    JsonDocument &doc = license.getLicenseJSON();
    return doc["data"]["panel"] | 1; // default 1 panel
}

bool isWifiEnabled()
{
    JsonDocument &doc = license.getLicenseJSON();
    return doc["data"]["wifi"] | false;
}

int getMaxBrightness()
{
    JsonDocument &doc = license.getLicenseJSON();
    return doc["data"]["brightness"] | 30; // default 30%
}

// ------------------------------------------------------------
//  EN: Setup function
//  ID: Fungsi setup
// ------------------------------------------------------------
void setup()
{
    // EN: Start Serial communication
    // ID: Mulai komunikasi Serial
    Serial.begin(115200);
    Serial.println();
    Serial.println("\n=== FEATURE FLAGS DEMO ===\n");

    // EN: Device info
    // ID: Info perangkat
    auto info = license.getDeviceInfo();
    Serial.println("MODEL       : " + info.chipModel);
    Serial.println("MAC         : " + info.mac);
    Serial.println("FLASH SIZE  : " + info.flashSize);

    // EN: Device ID
    // ID: Device ID
    String deviceID = license.generateDeviceID(PRODUCT_SECRET);
    Serial.println("DeviceID : " + deviceID);
    Serial.println();

    // EN: Load license, u can change file name
    // ID: Muat lisensi, anda bisa mengganti nama filenya
    if (!license.loadLicense("/license.json"))
    {
        Serial.println("👀 License not found");
        return;
    }

    // EN: HMAC → verification use BearSSL (Recomended for ESP8266)
    // ID: HMAC → verifikasimenggunakan BearSSL (Direkomendasikan untuk ESP8266)
    LicenseStatus status = license.verifyLicense(
        HMAC_SECRET,     // HMAC crypto key
        PRODUCT_SECRET); // Device binding secret

    // EN: License is valid
    // ID: Lisensi valid
    if (status == FEL_LICENSE_OK)
    {
        Serial.println("✅ License Valid");
    }
    else
    {
        Serial.println("❌ License Invalid");
    }

    // EN: Print license data
    // ID: Cetak data lisensi
    license.printLicenseData(Serial);

    // =========================================================
    // FEATURE FLAGS
    // =========================================================
    // EN: Read feature flags from license
    // ID: Baca feature flags dari lisensi
    int licensedPanels = getLicensedPanelLimit();
    bool wifiAllowed = isWifiEnabled();
    int maxBrightness = getMaxBrightness();

    Serial.println("===== LICENSE FEATURES =====");
    Serial.print("Max Panel       : ");
    Serial.println(licensedPanels);
    Serial.print("WiFi Enabled    : ");
    Serial.println(wifiAllowed ? "YES" : "NO");
    Serial.print("Max Brightness  : ");
    Serial.println(maxBrightness);
    Serial.println("============================\n");

    // =========================================================
    // APPLY PANEL LIMIT
    // =========================================================
    // EN: Apply panel limit
    // ID: Terapkan batas panel
    if (installedPanels > licensedPanels)
    {
        Serial.println("⚠ Panel melebihi lisensi!");
        Serial.print("Panel aktif dibatasi ke : ");
        Serial.println(licensedPanels);
        installedPanels = licensedPanels;
    }

    Serial.print("Panel aktif sekarang : ");
    Serial.println(installedPanels);
    Serial.println();

    // =========================================================
    // APPLY WIFI FEATURE
    // =========================================================
    // EN: Apply WiFi feature
    // ID: Terapkan fitur WiFi
    if (wifiAllowed)
    {
        Serial.println("📡 WiFi STARTED");
    }
    else
    {
        Serial.println("🚫 WiFi DISABLED BY LICENSE");
    }

    Serial.println();

    // =========================================================
    // APPLY BRIGHTNESS LIMIT
    // =========================================================
    // EN: Apply brightness limit
    // ID: Terapkan batas brightness
    if (userBrightness > maxBrightness)
    {
        Serial.println("⚠ Brightness melebihi lisensi!");
        userBrightness = maxBrightness;
    }

    Serial.print("Brightness aktif : ");
    Serial.println(userBrightness);
}

// EN: Loop function
// ID: Fungsi loop
void loop()
{
    // EN: Main Program Loop
    // ID: Program Utama Loop
}
