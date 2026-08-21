#!/bin/bash
echo "Generating ECDSA key pair..."

openssl ecparam -name prime256v1 -genkey -noout -out private_key.pem
openssl ec -in private_key.pem -pubout -out public_key.pem

echo "Done!"
echo "Replace keys in Arduino sketch and license generator."
