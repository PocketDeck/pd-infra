#!/bin/sh
set -e

DEPS="go nginx python3 gcc make"
REPOS="pd-core3 pd-web qr-gen"
TASKS="\
make -C qr-gen build/release/qr-gen
go -C pd-core3 build -o ./server ./cmd/server\
chmod +x pd-web/serve
"

echo Install dependencies:
sudo apt update
sudo apt install $DEPS -y

echo
echo Clone git repos:
for repo in $REPOS; do
	git clone "https://github.com/PocketDeck/$repo.git"
done

echo
echo Build targets:
while read -r cmd; do
	echo "$cmd:"
	sh -c "$cmd"
done << EOF
$TASKS
EOF
