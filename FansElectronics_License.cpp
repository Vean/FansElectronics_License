/* ============================================================
  FansElectronics License for ESP32 & ESP8266 Library
  ------------------------------------------------------------
  Writer      : Irfan Indra Kurniawan, ST
  Author      : Fans Electronics
  Created     : 2026-02-06
  Version     : 1.1.0
  Website     : https://fanselectronics.com
  Repository  : https://github.com/Vean/FansElectronics_License

  Universal offline licensing system for ESP32 & ESP8266
  Supports LIGHT, HMAC and ECDSA verification modes.

  Copyright (c) 2016-2026 Fans Electronics
============================================================ */

#include "FansElectronics_License.h"
#include <LittleFS.h>
#if defined(ESP32)
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "mbedtls/pk.h"
#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <BearSSLHelpers.h>
#include <bearssl/bearssl.h>
#endif

// =====================================================
// CONSTRUCTOR
// =====================================================
#if defined(ESP32)
#define FEL_JSON_INIT_LIST : licenseDoc(1024)
#else
#define FEL_JSON_INIT_LIST
#endif

FansElectronics_License::FansElectronics_License(uint8_t mode)
    FEL_JSON_INIT_LIST
{
#if defined(ESP8266)
  if (mode == ECDSA)
    _mode = HMAC;
  else
    _mode = mode;
#else
  _mode = mode;
#endif
}

// =====================================================
// UNIVERSAL SHA256 (ESP32 + ESP8266)
// =====================================================
void FEL_sha256(const String &input, uint8_t output[32])
{
#if defined(ESP32)

  mbedtls_sha256((const unsigned char *)input.c_str(), input.length(), output, 0);

#elif defined(ESP8266)

  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, input.c_str(), input.length());
  br_sha256_out(&ctx, output);

#endif
}

// =====================================================
// SIMPLE BASE64 DECODER (untuk signature)
// =====================================================
static const unsigned char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int FEL_base64_decode(uint8_t *out, const char *in, int len)
{
  int j = 0;
  int val = 0, valb = -8;

  for (int k = 0; k < len; k++)
  {
    unsigned char c = in[k];
    if (c == '=' || c == '\n' || c == '\r')
      break;

    const char *p = strchr((const char *)b64_table, c);
    if (!p)
      continue;

    val = (val << 6) + (p - (const char *)b64_table);
    valb += 6;

    if (valb >= 0)
    {
      out[j++] = (val >> valb) & 0xFF;
      valb -= 8;
    }
  }
  return j;
}

void FEL_securityDelay()
{
  // Delay anti tempering, 2 seconds
  delay(3000);
}

// =====================================================
// HMAC SHA256 (REAL IMPLEMENTATION - BYTE BASED)
// =====================================================
void FEL_hmac_sha256(const String &key, const String &message, uint8_t out[32])
{
  const uint8_t blockSize = 64;
  uint8_t keyBlock[blockSize];
  memset(keyBlock, 0, blockSize);

  // Step 1 — key normalization
  if (key.length() > blockSize)
  {
    FEL_sha256(key, keyBlock);
  }
  else
  {
    memcpy(keyBlock, key.c_str(), key.length());
  }

  uint8_t o_key_pad[blockSize];
  uint8_t i_key_pad[blockSize];

  for (int i = 0; i < blockSize; i++)
  {
    o_key_pad[i] = keyBlock[i] ^ 0x5c;
    i_key_pad[i] = keyBlock[i] ^ 0x36;
  }

  // Step 2 — inner hash = SHA256(i_key_pad || message)
  uint8_t innerBuf[blockSize + message.length()];
  memcpy(innerBuf, i_key_pad, blockSize);
  memcpy(innerBuf + blockSize, message.c_str(), message.length());

  uint8_t innerHash[32];
  FEL_sha256(String((char *)innerBuf).substring(0, blockSize + message.length()), innerHash);

  // Step 3 — outer hash = SHA256(o_key_pad || innerHash)
  uint8_t outerBuf[blockSize + 32];
  memcpy(outerBuf, o_key_pad, blockSize);
  memcpy(outerBuf + blockSize, innerHash, 32);

  FEL_sha256(String((char *)outerBuf).substring(0, blockSize + 32), out);
}

// =====================================================
// DEVICE INFO UNIVERSAL
// =====================================================
FEL_DeviceInfo FansElectronics_License::getDeviceInfo()
{
  FEL_DeviceInfo info;

#if defined(ESP32)
  uint64_t mac = ESP.getEfuseMac();
  char macStr[20];
  sprintf(macStr, "%04X%08X", (uint16_t)(mac >> 32), (uint32_t)mac);
  info.mac = macStr;

  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  info.chipModel = "ESP32";
  info.flashSize = String(spi_flash_get_chip_size());

#elif defined(ESP8266)
  info.mac = WiFi.macAddress();
  info.chipModel = "ESP8266";
  info.flashSize = String(ESP.getFlashChipRealSize());
#endif

  return info;
}

// =====================================================
// DEVICE ID GENERATOR (CUSTOMIZABLE)
// =====================================================
String FansElectronics_License::generateDeviceID(String secret, bool useFlashSize)
{
  FEL_DeviceInfo info = getDeviceInfo();

  // Urutan FINAL fingerprint:
  // CHIP_MODEL + MAC + FLASH_SIZE + SECRET
  String input = info.chipModel;
  input += info.mac;

  if (useFlashSize)
    input += info.flashSize;

  input += secret;

  uint8_t hash[32];
  FEL_sha256(input, hash);

  char out[65];
  for (int i = 0; i < 32; i++)
    sprintf(out + i * 2, "%02X", hash[i]);

  return String(out);
}

// =====================================================
// Decode XOR Obfuscated Secret (PROGMEM Safe)
// =====================================================
String FansElectronics_License::decodeSecret(const uint8_t *data, size_t len, uint8_t xorKey)
{
  char decoded[len + 1];

  for (size_t i = 0; i < len; i++)
  {
#if defined(ESP32)
    decoded[i] = pgm_read_byte(&data[i]) ^ xorKey;
#elif defined(ESP8266)
    decoded[i] = pgm_read_byte(&data[i]) ^ xorKey;
#endif
  }

  decoded[len] = '\0';
  return String(decoded);
}

// =====================================================
// LOAD LICENSE
// =====================================================
bool FansElectronics_License::loadLicense()
{
  // Check file system
#if defined(ESP32)
  if (!LittleFS.begin())
    return false;
#elif defined(ESP8266)
  if (!LittleFS.begin())
  {
    LittleFS.format();
    if (!LittleFS.begin())
      return false;
  }
#endif

  File f = LittleFS.open("/license.json", "r");
  if (!f)
    return false;

  String json = f.readString();
  f.close();

  if (deserializeJson(licenseDoc, json))
    return false;

  JsonObject dataObj = licenseDoc["data"];
  serializeJson(dataObj, licenseDataString);
  licenseSignature = licenseDoc["signature"].as<String>();

  // wajib ada device_id
  if (!dataObj["device_id"].is<String>())
    return false;

  return true;
}

// =====================================================
// Parse PUBLIC KEY PEM → Base64 only (PROGMEM SAFE)
// hasil harus identik dengan PHP loadPublicKeyAsString()
// =====================================================
String FansElectronics_License::parsePublicKeyToString(const char *pem)
{
  String out;
  out.reserve(200);

  char line[100];
  uint16_t idx = 0;
  uint16_t i = 0;

  while (true)
  {
    char c = pgm_read_byte(&pem[i++]);
    if (c == 0)
      break;

    // akhir baris (\n atau \r)
    if (c == '\n' || c == '\r')
    {
      line[idx] = '\0';

      // skip header & footer
      if (strstr(line, "BEGIN PUBLIC KEY") == NULL &&
          strstr(line, "END PUBLIC KEY") == NULL &&
          strlen(line) > 0)
      {
        // ambil base64 di baris ini
        out += line;
      }

      idx = 0;
      continue;
    }

    // simpan karakter ke buffer baris
    if (idx < sizeof(line) - 1)
      line[idx++] = c;
  }

  return out;
}

// =====================================================
// VERIFY LICENSE SIGNATURE
// =====================================================
bool FansElectronics_License::verifyLicense(const char *key)
{
  // ================= CACHE VERIFY =================
  if (_licenseChecked)
    return _licenseVerified;

  // ================= LIGHT MODE =================
  if (_mode == LIGHT)
  {
    // Signature not used in light mode
    // Always return true
    _licenseChecked = true;
    _licenseVerified = true;
    return true;
  }

  // Hash license data
  uint8_t hash[32];
  FEL_sha256(licenseDataString, hash);

  // Decode base64 signature
  uint8_t sig[128];
  int sig_len = FEL_base64_decode(sig, licenseSignature.c_str(), licenseSignature.length());

  // ================= HMAC MODE =================
  if (_mode == HMAC)
  {
    uint8_t calc[32];
    FEL_hmac_sha256(String(key), licenseDataString, calc);

    if (sig_len != 32)
    {
      //  Length mismatch
      _licenseChecked = true;
      _licenseVerified = false;
      FEL_securityDelay();
      return false;
    }

    for (int i = 0; i < 32; i++)
      if (sig[i] != calc[i])
      {
        // Not match
        _licenseChecked = true;
        _licenseVerified = false;
        FEL_securityDelay();
        return false;
      }
    // All good
    _licenseChecked = true;
    _licenseVerified = true;
    return true;
  }

#if defined(ESP32)
  // ================= ECDSA MODE =================
  if (_mode == ECDSA)
  {
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    if (mbedtls_pk_parse_public_key(&pk,
                                    (const unsigned char *)key,
                                    strlen(key) + 1) != 0)
    {
      // Failed to parse public key
      _licenseChecked = true;
      _licenseVerified = false;
      FEL_securityDelay();
      return false;
    }

    int ret = mbedtls_pk_verify(&pk,
                                MBEDTLS_MD_SHA256,
                                hash, 0,
                                sig, sig_len);

    mbedtls_pk_free(&pk);
    if (ret == 0)
    {
      _licenseChecked = true;
      _licenseVerified = true;
      return true;
    }
    else
    {
      _licenseChecked = true;
      _licenseVerified = false;
      FEL_securityDelay();
      return false;
    }
  }
#endif
  //  ================= UNSUPPORTED MODE =================
  _licenseChecked = true;
  _licenseVerified = false;
  FEL_securityDelay();
  return false;
}

// =====================================================
// CHECK IF LICENSE IS FOR THIS DEVICE
// =====================================================
bool FansElectronics_License::isLicenseForThisDevice(String secret, bool useFlashSize)
{
  // ambil device_id dari license
  String licensedID = getLicensedDeviceID();

  // generate device_id dari hardware sekarang
  String currentID = generateDeviceID(secret, useFlashSize);

  return licensedID == currentID;
}

// =====================================================
// GET MODE
// =====================================================
uint8_t FansElectronics_License::getMode()
{
  return _mode;
}

// =====================================================
// Helper: get value from license data
String FansElectronics_License::getString(const char *key)
{
  if (!licenseDoc["data"].containsKey(key))
    return "";

  JsonVariant v = licenseDoc["data"][key];

  if (v.is<const char *>())
    return String(v.as<const char *>());

  if (v.is<bool>())
    return v.as<bool>() ? "true" : "false";

  if (v.is<long>() || v.is<int>())
    return String(v.as<long>());

  if (v.is<float>() || v.is<double>())
    return String(v.as<double>(), 6);

  return "";
}
// =====================================================
// Helper: get value from license data as Boolean
bool FansElectronics_License::getBool(const char *key, bool defaultVal)
{
  if (!licenseDoc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = licenseDoc["data"][key];

  if (v.is<bool>())
    return v.as<bool>();

  if (v.is<int>())
    return v.as<int>() != 0;

  if (v.is<const char *>())
  {
    String s = v.as<const char *>();
    s.toLowerCase();
    return (s == "true" || s == "1" || s == "yes");
  }

  return defaultVal;
}
// =====================================================
// Helper: get value from license data as Int
int FansElectronics_License::getInt(const char *key, int defaultVal)
{
  if (!licenseDoc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = licenseDoc["data"][key];

  if (v.is<int>() || v.is<long>())
    return v.as<int>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toInt();

  if (v.is<bool>())
    return v.as<bool>() ? 1 : 0;

  return defaultVal;
}

// =====================================================
// Helper: get value from license data as Float
float FansElectronics_License::getFloat(const char *key, float defaultVal)
{
  if (!licenseDoc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = licenseDoc["data"][key];

  if (v.is<float>() || v.is<double>())
    return v.as<float>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toFloat();

  if (v.is<int>())
    return (float)v.as<int>();

  return defaultVal;
}

// =====================================================
// Helper: get value from license data as Double
double FansElectronics_License::getDouble(const char *key, double defaultVal)
{
  if (!licenseDoc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = licenseDoc["data"][key];

  if (v.is<float>() || v.is<double>())
    return v.as<double>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toDouble();

  if (v.is<int>())
    return (double)v.as<int>();

  return defaultVal;
}

// Helper: check if key exists in license data
bool FansElectronics_License::hasKey(const char *key)
{
  return licenseDoc["data"].containsKey(key);
}

// Helper: access raw JSON license data
JSON_DOC_GLOBAL &FansElectronics_License::getLicenseJSON()
{
  return licenseDoc;
}
// Helper: get device_id from license
String FansElectronics_License::getLicensedDeviceID()
{
  return licenseDoc["data"]["device_id"].as<String>();
}
// Helper: debug print
void FansElectronics_License::printDebug(Stream &s)
{
  s.println("===== LICENSE DEBUG =====");
  if (_mode == HMAC)
    s.println("Mode        : HMAC");
  if (_mode == ECDSA)
    s.println("Mode        : ECDSA");
  if (_mode == LIGHT)
    s.println("Mode        : LIGHT");
  s.print("Device ID   : ");
  s.println(getLicensedDeviceID());
  s.print("Loaded      : ");
  s.println(!licenseDataString.isEmpty());
  s.println("=========================");
}
// Helper: print license data
void FansElectronics_License::printLicenseData(Stream &s)
{
  if (licenseDataString.isEmpty())
  {
    s.println("License not loaded");
    return;
  }

  s.println("===== LICENSE DATA =====");

  JsonObject data = licenseDoc["data"];

  for (JsonPair kv : data)
  {
    s.print(kv.key().c_str());
    s.print("\t: ");
    s.println(kv.value().as<String>());
  }

  s.println("========================");
}
