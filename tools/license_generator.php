<?php
/* =====================================================================
    FansElectronics License Generator (v2.1.0)
    --------------------------------------------------------------------
    Mode : WEB + CLI
    Usage WEB : license_generator.php?encryption=ECDSA_AES&aes_key=Rahasia&device_id=XXX
    Usage CLI : php license_generator.php encryption=ECDSA_AES aes_key=Rahasia device_id=XXX
===================================================================== */

header('Content-Type: application/json');

// =====================================================
// SUPPORT CLI MODE
// =====================================================
if (php_sapi_name() === 'cli') {
    parse_str(implode('&', array_slice($argv, 1)), $_GET);
}

// =====================================================
// HELPER: LOAD PUBLIC KEY → STRING (UNTUK HMAC)
// =====================================================
function loadPublicKeyAsString($path)
{
    if (!file_exists($path))
        die(json_encode(["error" => "public_key.pem not found"]));

    $pem = file_get_contents($path);
    $pem = str_replace("-----BEGIN PUBLIC KEY-----", "", $pem);
    $pem = str_replace("-----END PUBLIC KEY-----", "", $pem);
    $pem = str_replace(["\r", "\n", " "], "", $pem);

    return trim($pem);
}

// =====================================================
// PATH KEY
// =====================================================
$HMAC_SECRET = "MY_HMAC_SECRET";
$AES_SECRET  = "MY_AES_SECRET";

$basePath = __DIR__ . "/keys/";
$publicKeyPath  = $basePath . "public_key.pem";
$privateKeyPath = $basePath . "private_key.pem";

// =====================================================
// LOAD KEY
// =====================================================
if ($HMAC_SECRET == "") {
    $HMAC_SECRET = loadPublicKeyAsString($publicKeyPath);
}
$privateKey = null;
if (file_exists($privateKeyPath)) {
    $privateKey = openssl_pkey_get_private(file_get_contents($privateKeyPath));
}

// =====================================================
// GET MODE & AES KEY
// =====================================================
$mode = strtoupper($_GET['encryption'] ?? '');
$aes_key = $_GET['aes_key'] ?? $AES_SECRET;

if (!$mode) {
    die(json_encode(["error" => "encryption required (HMAC/ECDSA/LIGHT/HMAC_AES/ECDSA_AES/LIGHT_AES)"]));
}

$is_aes = strpos($mode, '_AES') !== false;
$base_mode = str_replace('_AES', '', $mode); // Mengekstrak base mode (HMAC, ECDSA, dll)

if ($is_aes && empty($aes_key)) {
    die(json_encode(["error" => "aes_key required for AES encryption modes"]));
}

// =====================================================
// GET LICENSE DATA (FREE PARAM)
// =====================================================
$data = $_GET;
unset($data['encryption'], $data['aes_key']); // Bersihkan parameter sistem dari payload

if (!isset($data['device_id'])) {
    die(json_encode(["error" => "device_id required"]));
}

// Data mentah JSON (Tanpa spasi agar identik dengan C++ serializeJson)
$jsonData = json_encode($data, JSON_UNESCAPED_SLASHES);

// =====================================================
// AES ENCRYPTION (JIKA MENGGUNAKAN MODE _AES)
// =====================================================
$payloadData = $jsonData;
$iv_string = "";

if ($is_aes) {
    // 1. Hash kata sandi AES menjadi persis 32-byte (256-bit)
    $key_32 = hash('sha256', $aes_key, true);

    // 2. Buat IV acak sepanjang 16 karakter (huruf/angka)
    $iv_string = substr(bin2hex(random_bytes(8)), 0, 16);

    // 3. Enkripsi dengan AES-256-CBC
    $encrypted = openssl_encrypt($jsonData, 'aes-256-cbc', $key_32, OPENSSL_RAW_DATA, $iv_string);

    // 4. Ubah payload menjadi teks Base64
    $payloadData = base64_encode($encrypted);
}

// =====================================================
// SIGNATURE
// =====================================================
$signature_base64 = "";

// ---------- HMAC ----------
if ($base_mode == "HMAC") {
    // Stempel berdasarkan $payloadData (Bisa JSON mentah, bisa teks Base64 AES)
    $signature_raw = hash_hmac("sha256", $payloadData, $HMAC_SECRET, true);
    $signature_base64 = base64_encode($signature_raw);
}
// ---------- ECDSA ----------
elseif ($base_mode == "ECDSA") {
    if (!$privateKey)
        die(json_encode(["error" => "private_key.pem missing"]));

    openssl_sign($payloadData, $signature, $privateKey, OPENSSL_ALGO_SHA256);
    $signature_base64 = base64_encode($signature);
}
// ---------- LIGHT ----------
elseif ($base_mode == "LIGHT") {
    $signature_base64 = "LIGHT_MODE";
} else {
    die(json_encode(["error" => "Invalid encryption mode"]));
}

// =====================================================
// OUTPUT LICENSE JSON
// =====================================================
$license = [
    "data" => $is_aes ? $payloadData : $data, // Jika AES, data berbentuk string tunggal
    "signature" => $signature_base64
];

// Sisipkan IV ke dalam JSON agar bisa dibaca oleh ESP32
if ($is_aes) {
    $license["iv"] = $iv_string;
}

// CLI → save file
if (php_sapi_name() === 'cli') {
    file_put_contents("license.json", json_encode($license, JSON_PRETTY_PRINT));
    echo "license.json generated successfully in $mode mode!\n";
    exit;
}

// WEB → output JSON
echo json_encode($license, JSON_PRETTY_PRINT);
