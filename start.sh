#!/bin/sh
set -e

echo "Starting Core backend..."
/app/pd-core/build/release/pd-core &

echo "Starting Web frontend..."
/app/pd-web/serve &

echo "Starting nginx..."
nginx -g "daemon off;"
