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

// EN: Define your product secret, 1 product has a different secret
// ID: Definisikan product secret Anda, 1 produk memiliki secret yang berbeda
#define PRODUCT_SECRET "YOUR_PRODUCT_SECRET"

// EN: Create FansElectronics_License object in LIGHT mode
// ID: Buat objek FansElectronics_License dalam mode LIGHT
FansElectronics_License license(LIGHT);

//  EN: Setup function
//  ID: Fungsi setup
void setup()
{
    // EN: Start Serial communication
    // ID: Mulai komunikasi Serial
    Serial.begin(115200);
    Serial.println();

    // EN: Device info
    // ID: Info perangkat
    auto info = license.getDeviceInfo();
    Serial.println("MODEL : " + info.chipModel);
    Serial.println("MAC   : " + info.mac);
    Serial.println("FLASH SIZE  : " + info.flashSize);

    // EN: Device ID
    // ID: Device ID
    String deviceID = license.generateDeviceID(PRODUCT_SECRET);
    Serial.println();
    Serial.println("====== THIS IS DEVICE ID ======");
    Serial.println(deviceID);
    Serial.println("===============================");
}

void loop() {}
