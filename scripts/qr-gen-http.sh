#!/bin/sh

QR_BIN="/app/qr-gen"
BODY=$(cat)
OUTPUT="$("$QR_BIN" "$BODY" L)"

echo "Content-Type: image/svg+xml"
echo ""
echo "$OUTPUT"
