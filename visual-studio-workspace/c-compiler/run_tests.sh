#!/bin/bash
set -uo pipefail
cd "$(dirname "$0")"

echo "Building compiler..."
gcc -g cc.c -o cc

fail=0
for f in ../test_*.c; do
  base=$(basename "$f" .c)
  echo "--- $base ---"
  ./cc "$f" > "$base.s" 2>cc.err || { echo "cc failed for $base"; sed -n '1,120p' cc.err; fail=1; continue; }
  gcc -o "$base.ours" "$base.s"
  gcc -g "$f" -o "$base.ref"
  ./$base.ours || true
  o=$?
  ./$base.ref || true
  r=$?
  echo "$base OURS:$o REF:$r"
  if [ $o -ne $r ]; then
    echo "Mismatch for $base"
    fail=1
  fi
done

if [ $fail -eq 0 ]; then
  echo "All tests passed"
else
  echo "Some tests failed"
  exit 1
fi
