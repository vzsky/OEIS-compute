#!/usr/bin/env bash
BASELINE_DIR="./benchmark/baseline"

BINARIES=$(find build -type f -executable -name '*_bench')

if [ -z "$BINARIES" ]; then
  echo "No benchmark binaries found in build/"
  exit 1
fi

for bench in $BINARIES; do
  name=$(basename "$bench" _bench)
  echo "Running benchmark: $name"

  BASELINE_FILE="$BASELINE_DIR/${name}_bench.json"
  FLAGS="--benchmark_out=$BASELINE_FILE --benchmark_out_format=json"

  "$bench" $FLAGS
  
done
