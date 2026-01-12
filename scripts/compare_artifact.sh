#!/usr/bin/env bash
set -euo pipefail

ARTIFACTS_DIR=./artifacts
BASELINE_DIR=./benchmark/baseline

status=0

for artifact in "$ARTIFACTS_DIR"/*; do
  file=$(basename "$artifact")
  base="${file%.*}"
  baseline="$BASELINE_DIR/${base}_bench.json"

  if [ -f "$baseline" ]; then
    if ! ./scripts/regression_test.py "$baseline" "$artifact"; then
      status=1
    fi
  else
    echo "Missing baseline for $file" >&2
    status=1
  fi
done

exit $status

