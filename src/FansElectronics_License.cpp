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
#include <bearssl/bearssl.h>
#endif

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

// =====================================================
// Delay anti tempering (misalnya saat verifikasi gagal)
// =====================================================
void FEL_securityDelay()
{
  // Delay anti tempering, 2 seconds
  delay(3000);
}

// =====================================================
// SHA256 helper (Aman untuk Data Biner / Tanpa String)
// =====================================================
void FEL_sha256_binary(const uint8_t *input, size_t length, uint8_t output[32])
{
#if defined(ESP32)
  mbedtls_sha256(input, length, output, 0);
#else
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, input, length);
  br_sha256_out(&ctx, output);
#endif
}

// Wrapper tetap dipertahankan agar tidak merusak fungsi lama Anda
void FEL_sha256(const String &input, uint8_t output[32])
{
  FEL_sha256_binary((const uint8_t *)input.c_str(), input.length(), output);
}

// =====================================================
// HMAC SHA256 (VERSI PERBAIKAN - AMAN KARAKTER '6' / 0x00)
// =====================================================
void FEL_hmac_sha256(const String &key, const String &message, uint8_t out[32])
{
  const uint8_t blockSize = 64;
  uint8_t keyBlock[blockSize];
  memset(keyBlock, 0, blockSize);

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

  size_t innerLen = blockSize + message.length();
  uint8_t innerBuf[innerLen];
  memcpy(innerBuf, i_key_pad, blockSize);
  memcpy(innerBuf + blockSize, message.c_str(), message.length());

  uint8_t innerHash[32];
  FEL_sha256_binary(innerBuf, innerLen, innerHash); // 🟢 Menggunakan penanganan biner murni

  size_t outerLen = blockSize + 32;
  uint8_t outerBuf[outerLen];
  memcpy(outerBuf, o_key_pad, blockSize);
  memcpy(outerBuf + blockSize, innerHash, 32);

  FEL_sha256_binary(outerBuf, outerLen, out); // 🟢 Menggunakan penanganan biner murni
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
                                                     uint8_t idLength,
                                                     bool useFlashSize)
{
  if (_licenseVerified)
    return FEL_LICENSE_OK;

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
    if (!isLicenseForDevice(productSecret, idLength, useFlashSize))
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
// DEVICE ID GENERATOR (CONFIGURABLE LENGTH)
// =====================================================
String FansElectronics_License::generateDeviceID(String secret,
                                                 uint8_t idLength,
                                                 bool useFlashSize)
{
  // ---- Safety clamp ----
  if (idLength < 16)
    idLength = 16;
  if (idLength > 64)
    idLength = 64;
  if (idLength % 2 != 0)
    idLength++; // must be even

  uint8_t bytesToUse = idLength / 2;

  FEL_DeviceInfo info = getDeviceInfo();

  // fingerprint = CHIP + MAC + FLASH + SECRET
  String input = info.chipModel;
  input += info.mac;

  if (useFlashSize)
    input += info.flashSize;

  input += secret;

  uint8_t hash[32];
  FEL_sha256(input, hash);

  // convert to HEX (custom length)
  char out[65];

  for (int i = 0; i < bytesToUse; i++)
    sprintf(out + i * 2, "%02X", hash[i]);

  out[bytesToUse * 2] = '\0';

  return String(out);
}

// =====================================================
// VERIFY SIGNATURE (HMAC mode)
// =====================================================
// =====================================================
// VERIFY SIGNATURE (Fixed v2.0.0 - Integration Mode)
// =====================================================
bool FansElectronics_License::verifySignature(const char *key)
{
  // 1. Jika mode LIGHT, signature tidak diverifikasi secara kriptografi
  if (_mode == LIGHT)
  {
    return true;
  }

  if (licenseSignature.isEmpty() || !key || strlen(key) == 0)
    return false;

  // 2. Buat Hash SHA256 dari data lisensi (diperlukan baik untuk HMAC maupun ECDSA)
  uint8_t hash[32];
  FEL_sha256(licenseDataString, hash);

  // 3. Decode signature berformat Base64 (menggunakan decoder bawaan v1.1.0 Anda)
  uint8_t sig[128];
  int sig_len = FEL_base64_decode(sig, licenseSignature.c_str(), licenseSignature.length());

  // ================= HMAC MODE (Khusus ESP8266 / Fallback) =================
  if (_mode == HMAC)
  {
    uint8_t calc[32];
    FEL_hmac_sha256(String(key), licenseDataString, calc);

    if (sig_len != 32)
    {
      FEL_securityDelay();
      return false;
    }

    for (int i = 0; i < 32; i++)
    {
      if (sig[i] != calc[i])
      {
        FEL_securityDelay();
        return false;
      }
    }
    return true; // HMAC Valid
  }

  // ================= ECDSA MODE (Khusus ESP32) =================
#if defined(ESP32)
  if (_mode == ECDSA)
  {
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    // Parse Public Key PEM yang dimasukkan pengguna
    if (mbedtls_pk_parse_public_key(&pk,
                                    (const unsigned char *)key,
                                    strlen(key) + 1) != 0)
    {
      mbedtls_pk_free(&pk);
      FEL_securityDelay();
      return false; // Gagal membaca Public Key
    }

    // Lakukan verifikasi ECDSA yang sesungguhnya
    int ret = mbedtls_pk_verify(&pk,
                                MBEDTLS_MD_SHA256,
                                hash, 0,
                                sig, sig_len);

    mbedtls_pk_free(&pk);

    if (ret == 0)
    {
      return true; // ECDSA Valid!
    }
    else
    {
      FEL_securityDelay();
      return false; // Signature tidak cocok / data telah diotak-atik
    }
  }
#endif

  // Jika masuk ke sini (misal memilih ECDSA di ESP8266), kunci ditolak
  FEL_securityDelay();
  return false;
}

// =====================================================
// CHECK IF LICENSE IS FOR THIS DEVICE (CONFIGURABLE ID LENGTH)
// =====================================================
bool FansElectronics_License::isLicenseForDevice(String secret,
                                                 uint8_t idLength,
                                                 bool useFlashSize)
{
  String licensedID = getLicensedDeviceID();
  String currentID = generateDeviceID(secret, idLength, useFlashSize);
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

// =====================================================
// Check license verification result
// =====================================================
bool FansElectronics_License::isValid()
{
  return _licenseVerified;
}

// =====================================================
// Check license loaded state
// =====================================================
bool FansElectronics_License::isLoaded()
{
  return !licenseDataString.isEmpty();
}

// =====================================================
// Get mode as readable string
// =====================================================
String FansElectronics_License::getModeString()
{
  if (_mode == LIGHT)
    return "LIGHT";
  if (_mode == HMAC)
    return "HMAC";
  if (_mode == ECDSA)
    return "ECDSA";
  return "UNKNOWN";
}

// =====================================================
// Get library version
// =====================================================
String FansElectronics_License::getLibraryVersion()
{
  return FEL_VERSION;
}