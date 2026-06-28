if [ "${1:-}" = "local" ]; then
  for f in log/*_bench.json; do
    mv "$f" "benchmark/local_baseline/$(basename "$f")"
  done
else
  for f in artifacts/*.json; do
    mv "$f" "benchmark/baseline/$(basename "${f%.json}_bench.json")"
  done
fi
