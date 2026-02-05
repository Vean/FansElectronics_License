#!/bin/bash
echo "Generating ECDSA key pair..."

openssl ecparam -name prime256v1 -genkey -noout -out keys/private_key.pem
openssl ec -in keys/private_key.pem -pubout -out keys/public_key.pem

echo "Done!"
echo "Replace keys in Arduino sketch and license generator."
