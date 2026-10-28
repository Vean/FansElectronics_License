/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Updated     : 2026-10-28
  Library     : FansElectronics_License v2.3.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Basic_ECDSA

  Copyright (c) 2016-2026 Fans Electronics
============================================================ */

// EN: Include FansElectronics_License library & ArduinoJSON
// ID: Masukkan library FansElectronics_License & ArduinoJSON
#include <ArduinoJson.h>
#include <FansElectronics_License.h>

// EN: Define your product secret
// ID: Definisikan product secret Anda
#define PRODUCT_SECRET "MY_SECRET"

// EN: Recomendation minimum memory, u can change it
// ID: Rekomendasi minimal memory, anda bisa memnggantinya
#define JSON_MEMORY 1024

// EN: Define your public key for ECDSA
// ID: Definisikan public key Anda untuk ECDSA
const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEu2s2XllDI4fywu45IFhddil+7Egn
hVwhQLYTEp7VjGE/OlCWzeVPrx8cQxl25Fw1497TsNyW3TP+QxwVXbnaZw==
-----END PUBLIC KEY-----
)KEY";

// EN: Set ESP8266 & ESP32 JSON Memory Type
// ID: Set ESP8266 & ESP32 JSON Tipe Memory
#if defined(ESP8266)
StaticJsonDocument<JSON_MEMORY> doc;
#elif defined(ESP32)
DynamicJsonDocument doc(JSON_MEMORY);
#endif

// EN: Create FansElectronics_License object in ECDSA mode
// ID: Buat objek FansElectronics_License dalam mode ECDSA
FansElectronics_License license(doc, FEL_MODE_ECDSA);

/*------------------------------------------------------------------------------
  EN: Setup function
  ID: Fungsi setup
*/
void setup()
{
  // EN: Start Serial communication
  // ID: Mulai komunikasi Serial
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== ECDSA MODE ===");

  // EN: Device info
  // ID: Info perangkat
  auto info = license.getDeviceInfo();
  Serial.println("MODEL       : " + info.chipModel);
  Serial.println("MAC         : " + info.mac);
  Serial.println("FLASH SIZE  : " + info.flashSize);

  // EN: Device ID
  // ID: Device ID
  String deviceID = license.generateDeviceID(PRODUCT_SECRET);
  Serial.println("DeviceID    : " + deviceID);
  Serial.println();

  // EN: Load license, u can change file name
  // ID: Muat lisensi, anda bisa mengganti nama filenya
  if (!license.loadLicense("/license.json"))
  {
    Serial.println("👀 License not found");
    return;
  }

  // EN: ECDSA → verification use mbedTLS (Recommended for ESP32)
  // ID: ECDSA → verifikasi menggunakan mbedTLS (Direkomendasikan untuk ESP32)
  LicenseStatus status = license.verifyLicense(
      PUBLIC_KEY,      // Your Public crypto key
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

#ifdef FEL_ENABLE_DEBUG
  // EN: Print license data
  // ID: Cetak data lisensi
  license.printLicenseData(Serial);
#endif

  // EN: End of setup
  // ID: Akhir dari setup
}

/*------------------------------------------------------------------------------
  EN: Loop function
  ID: Fungsi loop
*/
void loop()
{
  // EN: Runtime hardware entropy token verification
  // ID: Verifikasi token runtime berbasis hardware entropy
  if (license.isValid())
  {
    // EN: Main Program Loop (License Valid)
    // ID: Program Utama Loop (Lisensi Sah)
  }
  else
  {
    // EN: System Locked / Unauthorized
    // ID: Sistem Terkunci / Tidak Berlisensi
  }

  delay(1000);
}