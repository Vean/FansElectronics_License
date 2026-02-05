/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Library     : FansElectronics_License v1.0.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Basic_HMAC

  Copyright (c) 2016-2026 Fans Electronics
============================================================ */

// EN: Include FansElectronics_License library
// ID: Masukkan library FansElectronics_License
#include <FansElectronics_License.h>

// EN: Define your product secret
// ID: Definisikan product secret Anda
#define PRODUCT_SECRET "MY_SECRET"

// EN: Define your HMAC secret
// ID: Definisikan HMAC secret Anda
#define HMAC_SECRET "MY_HMAC_SECRET"

// EN: Create FansElectronics_License object in HMAC mode
// ID: Buat objek FansElectronics_License dalam mode HMAC
FansElectronics_License license(HMAC);

//  EN: Setup function
//  ID: Fungsi setup
void setup()
{
  // EN: Start Serial communication
  // ID: Mulai komunikasi Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== HMAC MODE ===");

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

  // EN: Load license
  // ID: Muat lisensi
  if (!license.loadLicense())
  {
    Serial.println("👀 License not found");
    return;
  }

  // EN: Verify license use method with HMAC secret
  // ID: Verifikasi lisensi menggunakan metode dengan HMAC secret
  if (!license.verifyLicense(HMAC_SECRET))
  {
    Serial.println("❌ License invalid");
    return;
  }

  // EN: Check if license is for this device
  // ID: Periksa apakah lisensi untuk perangkat ini
  if (!license.isLicenseForThisDevice(PRODUCT_SECRET))
  {
    // EN: License not for this device
    // ID: Lisensi bukan untuk perangkat ini
    Serial.println("❌ License not for this device!");
    return;
  }

  // EN: License is valid
  // ID: Lisensi valid
  Serial.println("✅ License OK (HMAC)");

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
