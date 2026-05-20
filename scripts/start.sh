#!/bin/sh
set -e

TASKS="\
   web-server at   ws://localhost:9002/ | /app/web/serve '0.0.0.0' '9002'
  core-server at http://localhost:9001/ | /app/core -port '9001'
reverse-proxy at http://localhost/      | nginx -c /app/nginx.conf\
"

RUNNING=$(
	SUBPID=$(sh -c 'echo $PPID')
	pgrep -fx "/bin/sh $0" | grep -vE "^($$|$SUBPID)$" || true
)

if [ -n "$RUNNING" ]; then
	echo -n "Cleaning up previous instances $RUNNING..."
	echo "$RUNNING" | xargs kill 2>/dev/null
	echo OK
fi

cleanup() {
	echo -n "Shutting down..."
	kill $pids 2>/dev/null || true
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

set +e

for pid in $pids; do
	wait "$pid"
done
