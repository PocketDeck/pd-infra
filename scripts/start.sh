#!/bin/sh
set -e

echo "Starting Core backend..."
/app/core &

echo "Starting Web frontend..."
/app/pd-web/serve &

echo "Starting nginx..."
nginx -g "daemon off;"
