for f in artifacts/*.json; do
  mv "$f" "benchmark/baseline/$(basename "${f%.json}_bench.json")"
done

