#!/usr/bin/env bash
BASELINE_DIR="./benchmark/baseline"

BINARIES=$(find build -type f -executable -name '*_bench')

if [ -z "$BINARIES" ]; then
  echo "No benchmark binaries found in build/"
  exit 1
fi

status=0
for bench in $BINARIES; do
  name=$(basename "$bench" _bench)
  echo "Running benchmark: $name"

  CURRENT_FILE="/tmp/$name.json"
  BASELINE_FILE="$BASELINE_DIR/${name}_bench.json"
  FLAGS="--benchmark_out=$CURRENT_FILE --benchmark_out_format=json"

  # Run the benchmark and capture output
  output=$("$bench" $FLAGS 2>&1)
  check=$("./scripts/regression_test.py" "$BASELINE_FILE" "$CURRENT_FILE")

  # Check if NEED HUMAN appears
  echo "$check"
  $check | grep "NEED HUMAN"
  if echo "$check" | grep -q "NEED HUMAN"; then
    echo "> NEED HUMAN to inspect $name"
    status=1
  fi
  
done

exit $status

