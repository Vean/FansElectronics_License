/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library - Example
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Updated     : 2026-08-21
  Library     : FansElectronics_License v2.1.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License
  Example     : Universal_Tester_ESP8266

  Copyright (c) 2016-2026 Fans Electronics
============================================================ */

// EN: Include FansElectronics_License library & ArduinoJSON
// ID: Masukkan library FansElectronics_License & ArduinoJSON
#include <ArduinoJson.h>
#include <FansElectronics_License.h>

// =========================================================
// EN: CHANGE MODE HERE FOR TESTING (Choose one)
// ID: UBAH MODE DI SINI UNTUK TESTING (Pilih salah satu)
// =========================================================
#define FEL_CURRENT_MODE FEL_MODE_LIGHT
// #define FEL_CURRENT_MODE FEL_MODE_HMAC
// #define FEL_CURRENT_MODE FEL_MODE_LIGHT_AES
// #define FEL_CURRENT_MODE FEL_MODE_HMAC_AES

// =========================================================
// EN: SECRET KEYS CONFIGURATION
// ID: KONFIGURASI KUNCI RAHASIA
// =========================================================
// EN: Define your product secret
// ID: Definisikan product secret Anda
#define PRODUCT_SECRET "MY_SECRET"
// EN: Define your HMAC secret
// ID: Definisikan HMAC secret Anda
#define HMAC_SECRET "MY_HMAC_SECRET"
// EN: Define your AES secret
// ID: Definisikan AES secret Anda
#define AES_SECRET "MY_AES_SECRET"

// EN: Specific RAM allocation for ESP8266 (Using Static to not overload Heap)
// ID: Alokasi RAM khusus ESP8266 (Menggunakan Static agar tidak membebani Heap)
#define JSON_MEMORY 1024
StaticJsonDocument<JSON_MEMORY> doc;

// EN: Initialize License Object
// ID: Inisialisasi Objek License
FansElectronics_License license(doc, FEL_CURRENT_MODE);

// EN: Setup function
// ID: Fungsi setup
void setup()
{
  // EN: Delay to ensure serial readiness
  // ID: Jeda untuk memastikan kesiapan serial
  delay(1000);

  // EN: Start Serial communication
  // ID: Mulai komunikasi Serial
  Serial.begin(115200);
  Serial.println();

  // EN: Print current testing mode
  // ID: Cetak mode pengujian saat ini
  Serial.println("\n=================================");
  Serial.print("MENGUJI MODE : ");
  Serial.println(license.getModeString());
  Serial.println("=================================");

  // EN: Device info
  // ID: Info perangkat
  auto info = license.getDeviceInfo();
  Serial.println("MODEL       : " + info.chipModel);
  Serial.println("MAC         : " + info.mac);
  Serial.println("FLASH SIZE  : " + info.flashSize);

  // EN: Generate Device ID
  // ID: Buat Device ID
  String deviceID = license.generateDeviceID(PRODUCT_SECRET, FEL_ID_LEN_64, true);
  Serial.println("DeviceID    : " + deviceID);
  Serial.println();

  // EN: Load license, you can change the file name
  // ID: Muat lisensi, Anda bisa mengganti nama filenya
  if (!license.loadLicense("/license.json"))
  {
    Serial.println("👀 File /license.json tidak ditemukan atau rusak!");
    return;
  }

  // EN: Execution Verification (BearSSL)
  // ID: --- Verifikasi Eksekusi (BearSSL) ---
  LicenseStatus status = license.verifyLicense(
      // EN: ESP8266 purely uses HMAC Secret for its signature
      // ID: ESP8266 murni menggunakan HMAC Secret untuk stempelnya
      HMAC_SECRET,
      PRODUCT_SECRET,
      FEL_ID_LEN_64,
      true,
      // EN: AES-256 Key
      // ID: Kunci AES-256
      AES_SECRET);

  // EN: Final Result
  // ID: --- Hasil Akhir ---
  if (status == FEL_LICENSE_OK)
  {
    // EN: License Valid (Access Granted)
    // ID: License Valid (Akses Diberikan)
    Serial.println("✅ License Valid (Akses Diberikan)");
  }
  else
  {
    // EN: License Invalid! Error Code:
    // ID: License Invalid! Error Code:
    Serial.print("❌ License Invalid! Error Code: ");
    Serial.println(status);
  }

  // EN: Print Data (Proves successful BearSSL decryption)
  // ID: Cetak Data (Membuktikan dekripsi BearSSL sukses)
  license.printLicenseData(Serial);
}

// EN: Loop function
// ID: Fungsi loop
void loop() {}