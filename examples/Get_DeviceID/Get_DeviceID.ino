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

    /*
    EN: Device ID is formed by combination of:
    - Chip Model (Automatically taken according to ESP32 / ESP8266 device)
    - MAC Address (Automatically taken from WiFi MAC Address, unique for each device)
    - Flash Size (Optional, can be ignored if not needed change true / false on second parameter)
    - Product Secret (You must define it yourself, different for each of your products)
    ID: Device ID dapat bentuk dengan gambungan:
    - Chip Model (Otomatis diambil sesuai perangkat ESP32 / ESP8266)
    - MAC Address (Diambil otomatis dari WiFi MAC Address, unik untuk setiap perangkat)
    - Flash Size (Opsi, dapat diabaikan jika tidak diperlukan ganti true / false pada parameter kedua)
    - Product Secret (Anda harus mendefinisikan sendiri, berbeda untuk setiap produk Anda)
    */

    String deviceID = license.generateDeviceID(PRODUCT_SECRET, true);
    Serial.println();
    Serial.println("====== THIS IS DEVICE ID ======");
    Serial.println(deviceID);
    Serial.println("===============================");
}

void loop() {}
