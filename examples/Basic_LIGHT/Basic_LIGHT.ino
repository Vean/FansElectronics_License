/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Updated     : 2026-02-13
  Library     : FansElectronics_License v2.0.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Basic_LIGHT

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

// EN: Set ESP8266 & ESP32 JSON Memory Type
// ID: Set  ESP8266 & ESP32 JSON Tipe Memory
#if defined(ESP8266)
StaticJsonDocument<JSON_MEMORY> doc;
#elif defined(ESP32)
DynamicJsonDocument doc(JSON_MEMORY);
#endif

// EN: Create FansElectronics_License object in LIGHT mode
// ID: Buat objek FansElectronics_License dalam mode LIGHT
FansElectronics_License license(doc, LIGHT); // JSON doc, Encryption Type

//  EN: Setup function
//  ID: Fungsi setup
void setup()
{
  // EN: Start Serial communication
  // ID: Mulai komunikasi Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== LIGHT MODE ===");

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

  // EN: LIGHT → verification use Device ID Only
  // ID: LIGHT → verifikasi hanya menggunakan Device ID saja
  LicenseStatus status = license.verifyLicense(
      "",              // no crypto key
      PRODUCT_SECRET); // device binding secret

  // EN: Response License
  // ID: Respon Lisensi
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

  // EN: End of setup
  // ID: Akhir dari setup
}

// EN: Loop function
// ID: Fungsi loop
void loop()
{
  // EN: Main Program Loop
  // ID: Program Utama Loop
}
