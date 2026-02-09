<?php
// ======================================================
// FansElectronics Secret Encoder
// Convert plain text secret → XOR byte array for firmware
// How to Use CLI: php encode_secret.php MY_SUPER_SECRET_123 96
// ======================================================

if ($argc < 2) {
    echo "Usage:\n";
    echo "php encode_secret.php YOUR_SECRET_STRING\n";
    exit;
}

$secret = $argv[1];
$xorKey = $argv[2];

echo "\nOriginal Secret:\n$secret\n\n";
echo "// Encoded Array:\n";

echo "const uint8_t SECRET_OBF[] PROGMEM = {\n  ";

$bytes = [];
for ($i = 0; $i < strlen($secret); $i++) {
    $encoded = ord($secret[$i]) ^ $xorKey;
    $bytes[] = "0x" . strtoupper(str_pad(dechex($encoded), 2, '0', STR_PAD_LEFT));
}

echo implode(", ", $bytes);
echo "\n};\n\n";

echo "// XOR KEY:\n#define XOR_KEY $xorKey\n";
