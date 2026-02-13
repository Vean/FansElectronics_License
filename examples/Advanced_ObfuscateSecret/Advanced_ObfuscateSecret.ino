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
  Example     : Obfuscated_Secret

  ============================================================

  WHY THIS EXAMPLE EXISTS
  ------------------------------------------------------------
  EN:
  Storing PRODUCT_SECRET as plain text inside firmware is risky.
  Anyone can dump flash memory and read the secret easily.

  This example shows a SIMPLE OBFUSCATION technique to make
  reverse engineering much harder.

  This is NOT military grade encryption.
  This is firmware hardening.

  ID:
  Menyimpan PRODUCT_SECRET dalam bentuk teks biasa sangat berbahaya.
  Hacker bisa dump firmware lalu membaca secret dengan mudah.

  Contoh ini menunjukkan teknik OBFUSCATION sederhana untuk
  mempersulit proses reverse engineering.

  Ini bukan enkripsi militer.
  Ini adalah hardening firmware.
============================================================ */

// EN: Include FansElectronics_License library & ArduinoJSON
// ID: Masukkan library FansElectronics_License & ArduinoJSON
#include <ArduinoJson.h>
#include <FansElectronics_License.h>

/* ============================================================
   STEP 1 — OBFUSCATED SECRET
============================================================ */

/*
EN:
We DO NOT store secret as string: "MY_SECRET"

Instead we store XOR encoded bytes.

Even if attacker dumps firmware,
they will NOT see readable text.

ID:
Kita TIDAK menyimpan secret sebagai string biasa.
Melainkan disimpan dalam bentuk byte yang di-XOR.
*/

// XOR key (can be any random number)
#define XOR_KEY 91

// "MY_SECRET" XOR encoded offline
const uint8_t SECRET_OBF[] PROGMEM = {
    0x17, 0x03, 0x05, 0x09, 0x0F, 0x0A, 0x1F,
    0x08, 0x05, 0x09, 0x1F, 0x19, 0x08, 0x1F,
    0x0E, 0x05, 0x6B, 0x68, 0x69};

// EN: Recomendation minimum memory, u can change it
// ID: Rekomendasi minimal memory, anda bisa memnggantinya
#define JSON_MEMORY 1024

/* ============================================================
   PUBLIC KEY (ECDSA)
============================================================ */

const char PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEu2s2XllDI4fywu45IFhddil+7Egn
hVwhQLYTEp7VjGE/OlCWzeVPrx8cQxl25Fw1497TsNyW3TP+QxwVXbnaZw==
-----END PUBLIC KEY-----
)KEY";

// EN: Set ESP8266 & ESP32 JSON Memory Type
// ID: Set  ESP8266 & ESP32 JSON Tipe Memory
#if defined(ESP8266)
StaticJsonDocument<JSON_MEMORY> doc;
#elif defined(ESP32)
DynamicJsonDocument doc(JSON_MEMORY);
#endif

// EN: Create FansElectronics_License object in ECDSA mode
// ID: Buat objek FansElectronics_License dalam mode ECDSA
FansElectronics_License license(doc, ECDSA); // JSON doc, Encryption Type

/* ============================================================
   SETUP
============================================================ */
void setup()
{
    Serial.begin(115200);
    Serial.println("\n=== OBFUSCATED SECRET MODE ===");

    /*
    EN:
    Function to decode secret at runtime.
    Secret only exists in RAM temporarily.

    ID:
    Fungsi decode secret saat runtime.
    Secret hanya muncul di RAM sementara.
    */
    String PRODUCT_SECRET = license.decodeSecret(
        SECRET_OBF,
        sizeof(SECRET_OBF),
        XOR_KEY);
    Serial.println("Secret      : " + PRODUCT_SECRET);
    Serial.println();

    /*
    EN:
    From this point, code works EXACTLY the same.
    User doesn't need to change anything else.

    ID:
    Setelah ini kode berjalan seperti biasa.
    User tidak perlu mengubah apapun lagi.
    */

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

    // EN: HMAC → verification use BearSSL (Recomended for ESP8266)
    // ID: HMAC → verifikasimenggunakan BearSSL (Direkomendasikan untuk ESP8266)
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