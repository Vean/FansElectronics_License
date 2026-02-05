<?php
/* =====================================================================
    FansElectronics License Generator
    --------------------------------------------------------------------
    Mode : WEB + CLI
    Usage WEB : license_generator.php?encryption=HMAC&device_id=XXX
    Usage CLI : php license_generator.php encryption=HMAC device_id=XXX
===================================================================== */

header('Content-Type: application/json');


// =====================================================
// SUPPORT CLI MODE
// =====================================================
if (php_sapi_name() === 'cli') {
    parse_str(implode('&', array_slice($argv, 1)), $_GET);
}


// =====================================================
// HELPER: LOAD PUBLIC KEY → STRING (UNTUK HMAC)sh
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

$basePath = __DIR__ . "/keys/";
$publicKeyPath  = $basePath . "public_key.pem";
$privateKeyPath = $basePath . "private_key.pem";


// =====================================================
// LOAD KEY
// =====================================================
// auto load HMAC secret dari public key jika kosong
if ($HMAC_SECRET == "") {
    $HMAC_SECRET = loadPublicKeyAsString($publicKeyPath);
}
$privateKey = null;
if (file_exists($privateKeyPath)) {
    $privateKey = openssl_pkey_get_private(file_get_contents($privateKeyPath));
}


// =====================================================
// GET MODE
// =====================================================
$mode = strtoupper($_GET['encryption'] ?? '');

if (!$mode) {
    die(json_encode(["error" => "encryption required (HMAC/ECDSA/LIGHT)"]));
}


// =====================================================
// GET LICENSE DATA (FREE PARAM)
// =====================================================
$data = $_GET;
unset($data['encryption']);

if (!isset($data['device_id'])) {
    die(json_encode(["error" => "device_id required"]));
}

$jsonData = json_encode($data, JSON_UNESCAPED_SLASHES);


// =====================================================
// SIGNATURE
// =====================================================
$signature_base64 = "";

// ---------- HMAC ----------
if ($mode == "HMAC") {
    $signature_raw = hash_hmac("sha256", $jsonData, $HMAC_SECRET, true);
    $signature_base64 = base64_encode($signature_raw);
}

// ---------- ECDSA ----------
elseif ($mode == "ECDSA") {

    if (!$privateKey)
        die(json_encode(["error" => "private_key.pem missing"]));

    openssl_sign($jsonData, $signature, $privateKey, OPENSSL_ALGO_SHA256);
    $signature_base64 = base64_encode($signature);
}

// ---------- LIGHT ----------
elseif ($mode == "LIGHT") {
    $signature_base64 = "LIGHT_MODE";
} else {
    die(json_encode(["error" => "Invalid encryption mode"]));
}


// =====================================================
// OUTPUT LICENSE JSON
// =====================================================
$license = [
    "data" => $data,
    "signature" => $signature_base64
];


// CLI → save file
if (php_sapi_name() === 'cli') {
    file_put_contents("license.json", json_encode($license, JSON_PRETTY_PRINT));
    echo "license.json generated!\n";
    exit;
}

// WEB → output JSON
echo json_encode($license, JSON_PRETTY_PRINT);
