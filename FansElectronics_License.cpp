#include "FansElectronics_License.h"
#include <LittleFS.h>

#if defined(ESP32)
#include "mbedtls/sha256.h"
#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <bearssl/bearssl.h>
#endif

// =====================================================
// SHA256 helper
// =====================================================
void FEL_sha256(const String &input, uint8_t output[32])
{
#if defined(ESP32)
  mbedtls_sha256((const unsigned char *)input.c_str(), input.length(), output, 0);
#else
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, input.c_str(), input.length());
  br_sha256_out(&ctx, output);
#endif
}

// =====================================================
// Constructor
// =====================================================
FansElectronics_License::FansElectronics_License(JsonDocument &doc, uint8_t mode)
    : _doc(doc)
{
#if defined(ESP8266)
  _mode = (mode == ECDSA) ? HMAC : mode;
#else
  _mode = mode;
#endif
}

// =====================================================
// LOAD LICENSE FROM LITTLEFS
// =====================================================
bool FansElectronics_License::loadLicense(const char *path)
{
  if (!LittleFS.begin())
    return false;

  File f = LittleFS.open(path, "r");
  if (!f)
    return false;

  if (deserializeJson(_doc, f))
    return false;

  JsonObject dataObj = _doc["data"];
  serializeJson(dataObj, licenseDataString);
  licenseSignature = _doc["signature"].as<String>();

  _licenseLoaded = true;
  return true;
}

// =====================================================
// VERIFY LICENSE
// =====================================================
LicenseStatus FansElectronics_License::verifyLicense(const char *cryptoKey,
                                                     String productSecret,
                                                     bool useFlashSize)
{
  if (!_licenseLoaded)
    return FEL_LICENSE_JSON_INVALID;

  JsonObject dataObj = _doc["data"];
  if (dataObj.isNull())
    return FEL_LICENSE_MISSING_DATA_OBJECT;

  if (!dataObj["device_id"].is<const char *>())
    return FEL_LICENSE_MISSING_DEVICE_ID;

  if (!verifySignature(cryptoKey))
    return FEL_LICENSE_SIGNATURE_INVALID;

  if (productSecret.length() > 0)
    if (!isLicenseForDevice(productSecret, useFlashSize))
      return FEL_LICENSE_DEVICE_MISMATCH;

  _licenseVerified = true;
  return FEL_LICENSE_OK;
}

// =====================================================
// DEVICE INFO
// =====================================================
FEL_DeviceInfo FansElectronics_License::getDeviceInfo()
{
  FEL_DeviceInfo info;
#if defined(ESP32)
  uint64_t mac = ESP.getEfuseMac();
  char macStr[20];
  sprintf(macStr, "%04X%08X", (uint16_t)(mac >> 32), (uint32_t)mac);
  info.mac = macStr;
  info.chipModel = "ESP32";
  info.flashSize = String(spi_flash_get_chip_size());
#else
  info.mac = WiFi.macAddress();
  info.chipModel = "ESP8266";
  info.flashSize = String(ESP.getFlashChipRealSize());
#endif
  return info;
}

// =====================================================
// DEVICE ID GENERATOR
// =====================================================
String FansElectronics_License::generateDeviceID(String secret, bool useFlashSize)
{
  FEL_DeviceInfo info = getDeviceInfo();
  String input = info.chipModel + info.mac;
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
// VERIFY SIGNATURE (HMAC mode)
// =====================================================
bool FansElectronics_License::verifySignature(const char *key)
{
  uint8_t hash[32];
  FEL_sha256(licenseDataString, hash);
  return licenseSignature.length() > 10; // simplified stub (same logic bisa ditambah ECDSA lagi)
}

// =====================================================
// Check License for this Device ID
// =====================================================
bool FansElectronics_License::isLicenseForDevice(String secret, bool useFlashSize)
{
  String licensedID = getLicensedDeviceID();
  String currentID = generateDeviceID(secret, useFlashSize);
  return licensedID == currentID;
}

// =====================================================
// Get Value JSON License key as String
// =====================================================
String FansElectronics_License::getString(const char *key)
{
  if (!_doc["data"].containsKey(key))
    return "";

  JsonVariant v = _doc["data"][key];

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
// Get Value JSON License key as Boolean
// =====================================================
bool FansElectronics_License::getBool(const char *key, bool defaultVal)
{
  if (!_doc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = _doc["data"][key];

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
// Get Value JSON License key as Integer
// =====================================================
int FansElectronics_License::getInt(const char *key, int defaultVal)
{
  if (!_doc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = _doc["data"][key];

  if (v.is<int>() || v.is<long>())
    return v.as<int>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toInt();

  if (v.is<bool>())
    return v.as<bool>() ? 1 : 0;

  return defaultVal;
}

// =====================================================
// Get Value JSON License key as Float
// =====================================================
float FansElectronics_License::getFloat(const char *key, float defaultVal)
{
  if (!_doc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = _doc["data"][key];

  if (v.is<float>() || v.is<double>())
    return v.as<float>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toFloat();

  if (v.is<int>())
    return (float)v.as<int>();

  return defaultVal;
}

// =====================================================
// Get Value JSON License key as Double
// =====================================================
double FansElectronics_License::getDouble(const char *key, double defaultVal)
{
  if (!_doc["data"].containsKey(key))
    return defaultVal;

  JsonVariant v = _doc["data"][key];

  if (v.is<float>() || v.is<double>())
    return v.as<double>();

  if (v.is<const char *>())
    return String(v.as<const char *>()).toDouble();

  if (v.is<int>())
    return (double)v.as<int>();

  return defaultVal;
}

// =====================================================
// Check Existing Key
// =====================================================
bool FansElectronics_License::hasKey(const char *key)
{
  return _doc["data"].containsKey(key);
}

// =====================================================
// Get JSON License
// =====================================================
JsonDocument &FansElectronics_License::getLicenseJSON()
{
  return _doc;
}

// =====================================================
// Get Device ID License
// =====================================================
String FansElectronics_License::getLicensedDeviceID()
{
  return _doc["data"]["device_id"].as<String>();
}

// =====================================================
// Printing Debugging Verify License
// =====================================================
void FansElectronics_License::printDebug(Stream &s)
{
  s.println("===== LICENSE DEBUG =====");
  s.print("Loaded      : ");
  s.println(_licenseLoaded);
  s.print("Verified    : ");
  s.println(_licenseVerified);
  s.println("=========================");
}

// =====================================================
// Printing Debug All Data License
// =====================================================
void FansElectronics_License::printLicenseData(Stream &s)
{
  if (!_licenseLoaded)
  {
    s.println("License not loaded");
    return;
  }

  s.println("===== LICENSE DATA =====");

  JsonObject data = _doc["data"];

  for (JsonPair kv : data)
  {
    s.print(kv.key().c_str());
    s.print("\t: ");
    s.println(kv.value().as<String>());
  }

  s.println("========================");
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