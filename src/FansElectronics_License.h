/*------------------------------------------------------------------------------
  FansElectronics License for ESP32 & ESP8266 Library
  ------------------------------------------------------------------------------
  Wrote by    : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Updated     : 2026-10-28
  Version     : 2.3.0
  Website     : https://www.fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License

  Universal offline licensing system for ESP32 & ESP8266
  Supports LIGHT, HMAC, ECDSA and AES verification modes.
*/

#pragma once

/*------------------------------------------------------------------------------
  Validasi Board Arsitektur
*/
#if !defined(ESP32) && !defined(ESP8266)
#error "FansElectronics_License supports only ESP32 and ESP8266."
#endif

#include <Arduino.h>
#include <ArduinoJson.h>

/*------------------------------------------------------------------------------
  Versi & Konfigurasi Preprocessor Keamanan
*/
#define FEL_VERSION "2.3.0"

// Aktifkan baris ini hanya saat fase development/debugging.
// Komentari baris ini saat rilis produksi agar teks debug hilang dari biner (.bin).
// #define FEL_ENABLE_DEBUG

/*------------------------------------------------------------------------------
  Konstanta Bit Strength Device ID
*/
#define FEL_ID_64BIT 16
#define FEL_ID_96BIT 24
#define FEL_ID_128BIT 32 // ⭐ Default
#define FEL_ID_192BIT 48
#define FEL_ID_256BIT 64

/*------------------------------------------------------------------------------
  Alias Panjang Karakter ID
*/
#define FEL_ID_LEN_16 FEL_ID_64BIT
#define FEL_ID_LEN_24 FEL_ID_96BIT
#define FEL_ID_LEN_32 FEL_ID_128BIT
#define FEL_ID_LEN_48 FEL_ID_192BIT
#define FEL_ID_LEN_64 FEL_ID_256BIT

/*------------------------------------------------------------------------------
  Tipe Mode Enkripsi & Verifikasi
*/
#define FEL_MODE_LIGHT 0
#define FEL_MODE_HMAC 1
#define FEL_MODE_ECDSA 2
#define FEL_MODE_LIGHT_AES 3
#define FEL_MODE_HMAC_AES 4
#define FEL_MODE_ECDSA_AES 5

/*------------------------------------------------------------------------------
  Daftar Status Kode Hasil Verifikasi
*/
enum LicenseStatus
{
  FEL_LICENSE_FILE_NOT_FOUND = 0,
  FEL_LICENSE_OK = 1,
  FEL_LICENSE_JSON_INVALID = 2,
  FEL_LICENSE_MISSING_DATA_OBJECT = 3,
  FEL_LICENSE_MISSING_SIGNATURE = 4,
  FEL_LICENSE_MISSING_DEVICE_ID = 5,
  FEL_LICENSE_SIGNATURE_INVALID = 6,
  FEL_LICENSE_DEVICE_MISMATCH = 7,
  FEL_LICENSE_AES_DECRYPT_FAIL = 8
};

/*------------------------------------------------------------------------------
  Struktur Informasi Identitas Hardware
*/
struct FEL_DeviceInfo
{
  String mac;
  String chipModel;
  String flashSize;
};

/*------------------------------------------------------------------------------
  Definisi Kelas Utama FansElectronics_License
*/
class FansElectronics_License
{
public:
  FansElectronics_License(JsonDocument &doc, uint8_t mode = FEL_MODE_HMAC);

  void clearCache();
  bool loadLicense(const char *path = "/license.json");

  LicenseStatus verifyLicense(const char *cryptoKey,
                              String productSecret = "",
                              uint8_t idLength = FEL_ID_128BIT,
                              bool useFlashSize = true,
                              const char *aesKey = NULL);

  FEL_DeviceInfo getDeviceInfo();
  String generateDeviceID(String secret, uint8_t idLength = FEL_ID_128BIT, bool useFlashSize = true);

  // ⭐ v2.2.0 Single Mode: 32-bit Multi-Byte Rolling XOR + Dynamic Salt
  String decodeSecret(const uint8_t *data, size_t len, uint32_t xorKey);

  String parsePublicKeyToString(const char *key);

  String getString(const char *key);
  bool getBool(const char *key, bool defaultVal = false);
  int getInt(const char *key, int defaultVal = 0);
  float getFloat(const char *key, float defaultVal = 0);
  double getDouble(const char *key, double defaultVal = 0);
  bool hasKey(const char *key);

  // Status verifikasi cepat di loop()
  bool isValid();
  bool isValid(const String &productSecret);

  bool isLoaded();
  String getModeString();
  String getLibraryVersion();

  JsonDocument &getLicenseJSON();
  String getLicensedDeviceID();

  // Generator Checksum Hardware Entropy (32-bit Token Mask)
  uint32_t getDeviceEntropy(const String &secret);

  /*----------------------------------------------------------------------------
    Fungsi Debugging (Inline Header Guard)
    Otomatis dibersihkan oleh compiler jika FEL_ENABLE_DEBUG tidak aktif
  ----------------------------------------------------------------------------*/
#if defined(FEL_ENABLE_DEBUG) && (FEL_ENABLE_DEBUG == 1 || FEL_ENABLE_DEBUG == true)

  inline void printDebug(Stream &s)
  {
    s.println(F("===== LICENSE DEBUG ====="));
    s.print(F("Loaded      : "));
    s.println(_licenseLoaded);
    s.print(F("Checksum    : 0x"));
    s.println(_authChecksum, HEX);
    s.print(F("Last Status : "));
    s.println((int)_lastStatus);
    s.println(F("========================="));
  }

  inline void printLicenseData(Stream &s = Serial)
  {
    if (!_licenseLoaded)
    {
      s.println(F("License not loaded"));
      return;
    }

    s.println(F("===== LICENSE DATA ====="));

    JsonObject data = _doc["data"];

    for (JsonPair kv : data)
    {
      s.print(kv.key().c_str());
      s.print(F("\t: "));
      s.println(kv.value().as<String>());
    }

    s.println(F("========================"));
  }

#else

  inline void printDebug(Stream &s) {}
  inline void printLicenseData(Stream &s = Serial) {}

#endif

private:
  JsonDocument &_doc;
  uint8_t _mode;
  bool _licenseLoaded = false;

  // Checksum dynamic entropy untuk menggantikan boolean statis
  uint32_t _authChecksum = 0;
  String _savedSecret = "";

  LicenseStatus _lastStatus = FEL_LICENSE_FILE_NOT_FOUND;

  String licenseDataString;
  String licenseSignature;

  bool verifySignature(const char *key);
  bool isLicenseForDevice(String secret, uint8_t idLength, bool useFlashSize);
};