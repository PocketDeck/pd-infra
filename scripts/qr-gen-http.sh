#!/bin/sh
set -e

QR_BIN="qr-gen/build/release/qr-gen"
BODY=$(cat)
OUTPUT="$("$QR_BIN" "$BODY" L)"

echo "Content-Type: image/svg+xml"
echo ""
echo "$OUTPUT"
