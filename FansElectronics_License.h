/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library
  ------------------------------------------------------------
  Wrote by    : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Version     : 1.1.0
  Website     : https://fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License

  Universal offline licensing system for ESP32 & ESP8266
  Supports LIGHT, HMAC and ECDSA verification modes.

  Copyright (c) 2016-2026 Fans Electronics
============================================================ */

#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#define FEL_VERSION "1.0.0"

#define LIGHT 0
#define HMAC 1
#define ECDSA 2

#if defined(ESP8266)
typedef StaticJsonDocument<1024> JSON_DOC_GLOBAL;
#elif defined(ESP32)
typedef DynamicJsonDocument JSON_DOC_GLOBAL;
#else
typedef StaticJsonDocument<1024> JSON_DOC_GLOBAL;
#endif

struct FEL_DeviceInfo
{
  String mac;
  String chipModel;
  String flashSize;
};

class FansElectronics_License
{
public:
  FansElectronics_License(uint8_t mode = ECDSA);
  FEL_DeviceInfo getDeviceInfo();
  String generateDeviceID(String secret, bool useFlashSize = true);
  String decodeSecret(const uint8_t *data, size_t len, uint8_t xorKey);
  String parsePublicKeyToString(const char *key);
  String getString(const char *key);
  bool getBool(const char *key, bool defaultVal = false);
  int getInt(const char *key, int defaultVal = 0);
  float getFloat(const char *key, float defaultVal = 0);
  double getDouble(const char *key, double defaultVal = 0);
  uint8_t getMode();
  bool loadLicense();
  bool verifyLicense(const char *key);
  bool isLicenseForThisDevice(String productSecret, bool useFlashSize = true);
  bool hasKey(const char *key);
  void printDebug(Stream &s);
  void printLicenseData(Stream &s);

  // akses raw JSON license data
  JSON_DOC_GLOBAL &getLicenseJSON();

  // helper: ambil device_id dari license
  String getLicensedDeviceID();

private:
  uint8_t _mode;
  bool _licenseVerified;
  bool _licenseChecked;
  String licenseDataString;
  String licenseSignature;
  JSON_DOC_GLOBAL licenseDoc;
};
