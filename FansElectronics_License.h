/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library
  ------------------------------------------------------------
  Wrote by    : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Updated     : 2026-02-13
  Version     : 2.0.0
  Website     : https://www.fanselectronics.com
  Repository  : https://www.github.com/Vean/FansElectronics_License

  Universal offline licensing system for ESP32 & ESP8266
  Supports LIGHT, HMAC and ECDSA verification modes.

  v2.0.0 Major changes:
  - Hybrid JSON memory model (user allocates memory)
  - ArduinoJson v6 public dependency
  - Compatible workflow with v1 (loadLicense, getters, debug)

============================================================ */

#pragma once

// =========================================================
// Supported platforms guard
// =========================================================
#if !defined(ESP32) && !defined(ESP8266)
#error "FansElectronics_License supports only ESP32 and ESP8266."
#endif

#include <Arduino.h>
#include <ArduinoJson.h>

#define FEL_VERSION "2.0.0"

#define LIGHT 0
#define HMAC 1
#define ECDSA 2

// =========================================================
// License verification result (GLOBAL ENUM)
// =========================================================
enum LicenseStatus
{
  FEL_LICENSE_OK = 0,
  FEL_LICENSE_JSON_INVALID,
  FEL_LICENSE_FILE_NOT_FOUND,
  FEL_LICENSE_MISSING_DATA_OBJECT,
  FEL_LICENSE_MISSING_SIGNATURE,
  FEL_LICENSE_MISSING_DEVICE_ID,
  FEL_LICENSE_SIGNATURE_INVALID,
  FEL_LICENSE_DEVICE_MISMATCH
};

struct FEL_DeviceInfo
{
  String mac;
  String chipModel;
  String flashSize;
};

class FansElectronics_License
{
public:
  // Constructor now receives JsonDocument from user
  FansElectronics_License(JsonDocument &doc, uint8_t mode = HMAC);

  // ===== Core workflow (compatible with v1) =====
  bool loadLicense(const char *path = "/license.json");
  LicenseStatus verifyLicense(const char *cryptoKey,
                              String productSecret = "",
                              bool useFlashSize = true);

  // ===== Device tools =====
  FEL_DeviceInfo getDeviceInfo();
  String generateDeviceID(String secret, bool useFlashSize = true);
  String decodeSecret(const uint8_t *data, size_t len, uint8_t xorKey);
  String parsePublicKeyToString(const char *key);

  // ===== JSON helpers (same UX as v1) =====
  String getString(const char *key);
  bool getBool(const char *key, bool defaultVal = false);
  int getInt(const char *key, int defaultVal = 0);
  float getFloat(const char *key, float defaultVal = 0);
  double getDouble(const char *key, double defaultVal = 0);
  bool hasKey(const char *key);

  void printDebug(Stream &s);
  void printLicenseData(Stream &s);

  JsonDocument &getLicenseJSON();
  String getLicensedDeviceID();

private:
  JsonDocument &_doc;
  uint8_t _mode;
  bool _licenseLoaded = false;
  bool _licenseVerified = false;

  String licenseDataString;
  String licenseSignature;

  bool verifySignature(const char *key);
  bool isLicenseForDevice(String secret, bool useFlashSize);
};
