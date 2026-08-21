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
  Example     : Universal_Tester_ESP32

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
// #define FEL_CURRENT_MODE FEL_MODE_LIGHT
// #define FEL_CURRENT_MODE FEL_MODE_HMAC
// #define FEL_CURRENT_MODE FEL_MODE_ECDSA
// #define FEL_CURRENT_MODE FEL_MODE_LIGHT_AES
// #define FEL_CURRENT_MODE FEL_MODE_HMAC_AES
#define FEL_CURRENT_MODE FEL_MODE_ECDSA_AES

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

// EN: Define your public key for ECDSA
// ID: Definisikan public key Anda untuk ECDSA
const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEu2s2XllDI4fywu45IFhddil+7Egn
hVwhQLYTEp7VjGE/OlCWzeVPrx8cQxl25Fw1497TsNyW3TP+QxwVXbnaZw==
-----END PUBLIC KEY-----
)KEY";

// EN: RAM allocation for ESP32
// ID: Alokasi RAM untuk ESP32
#define JSON_MEMORY 1024
DynamicJsonDocument doc(JSON_MEMORY);

// EN: Initialize License Object
// ID: Inisialisasi Objek License
FansElectronics_License license(doc, FEL_CURRENT_MODE);

// EN: Variable status code
// ID: Variable dari kode status
LicenseStatus status

    // EN: Setup function
    // ID: Fungsi setup
    void setup()
{
  // EN: Start Serial communication
  // ID: Mulai komunikasi Serial
  Serial.begin(115200);

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

  // EN: Auto-Select Crypto Key
  // ID: --- Auto-Select Crypto Key ---
  const char *activeCryptoKey = "";
  if (FEL_CURRENT_MODE == FEL_MODE_HMAC || FEL_CURRENT_MODE == FEL_MODE_HMAC_AES)
  {
    activeCryptoKey = HMAC_SECRET;
  }
  else if (FEL_CURRENT_MODE == FEL_MODE_ECDSA || FEL_CURRENT_MODE == FEL_MODE_ECDSA_AES)
  {
    activeCryptoKey = PUBLIC_KEY;
  }

  // EN: Execution Verification
  // ID: --- Verifikasi Eksekusi ---
  status = license.verifyLicense(
      // EN: Automatically use PUBLIC_KEY or HMAC_SECRET
      // ID: Otomatis pakai PUBLIC_KEY atau HMAC_SECRET
      activeCryptoKey,
      PRODUCT_SECRET,
      FEL_ID_LEN_64,
      true,
      // EN: AES Key (Automatically ignored if mode is not _AES)
      // ID: Kunci AES (Otomatis diabaikan jika mode bukan _AES)
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

  // EN: Print Data (Successful Decryption/Not)
  // ID: Cetak Data (Berhasil Dekripsi/Tidak)
  license.printLicenseData(Serial);
}

// EN: Loop function
// ID: Fungsi loop
void loop()
{ // EN: License Valid (Access Granted)
  // ID: License Valid (Akses Diberikan)
  if (status == FEL_LICENSE_OK)
  {
    Serial.println("✅ License Valid (Akses Diberikan)");
  }
  // EN: License Invalid! Error Code:
  // ID: License Invalid! Error Code:
  else
  {
    Serial.print("❌ License Invalid! Error Code: ");
    Serial.println(status);
  }

  // EN: Delay for 1 second
  // ID: Delay selama 1 detik
  delay(1000);
}