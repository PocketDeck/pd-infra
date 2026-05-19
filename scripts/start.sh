#!/bin/sh
set -e

TASKS="\
   web-server at   ws://localhost:8080/ | pd-web/serve '0.0.0.0' '8080'
  core-server at http://localhost:9090/ | pd-core3/server -port '9090'
reverse-proxy at http://localhost:8081/ | nginx -c pd-web/nginx.conf -p pd-web -g 'daemon off;'\
"

RUNNING=$(
	SUBPID=$(sh -c 'echo $PPID')
	pgrep -fx "/bin/sh $0" | grep -vE "^($$|$SUBPID)$"
)

if [ -n "$RUNNING" ]; then
	echo -n "Cleaning up previous instances $RUNNING..."
	echo "$RUNNING" | xargs kill 2>/dev/null
	echo OK
fi

cleanup() {
	echo -n "Shutting down..."
	kill $pids 2>/dev/null
	echo OK
	exit 0
}
trap cleanup INT TERM

pids=""

while IFS="|" read -r name cmd; do
	nohup sh -c "$cmd" >/dev/null 2>&1 &
	pids="$pids $!"
	echo "Started $name"
done << EOF
$TASKS
EOF

echo "Monitoring PIDs: $pids"
echo "Press Ctrl+C to stop."

for pid in $pids; do
	wait "$pid"
done
