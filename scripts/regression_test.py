#!/usr/bin/env python3

# https://markaicode.com/performance-regression-testing-cicd/
# compare_benchmarks.py
import json
import sys

# Load baseline and current results
with open(sys.argv[1], 'r') as f:
    baseline = json.load(f)
    
with open(sys.argv[2], 'r') as f:
    current = json.load(f)

# Set regression threshold (%)
REGRESSION_THRESHOLD = 5.0
found_regression = False
need_human = False;

print("Performance Comparison Report")
print("=============================\n")

# Compare each benchmark
for current_bench in current['benchmarks']:
    name = current_bench['name']
    
    # Find matching baseline benchmark
    baseline_bench = next((b for b in baseline['benchmarks'] if b['name'] == name), None)
    if not baseline_bench:
        print(f"⚠️  New benchmark: {name}")
        continue
        
    # Calculate change %
    baseline_time = baseline_bench['cpu_time']
    current_time = current_bench['cpu_time']
    change_pct = ((current_time - baseline_time) / baseline_time) * 100
    
    if change_pct > REGRESSION_THRESHOLD:
        status = "🔴 REGRESSION"
        found_regression = True
    elif change_pct < -REGRESSION_THRESHOLD:
        status = "🟢 IMPROVEMENT"
    else:
        status = "⚪ NEUTRAL"
        
    print(f"{status}: {name}")
    print(f"  Baseline: {baseline_time:.2f} ns")
    print(f"  Current:  {current_time:.2f} ns")
    print(f"  Change:   {change_pct:+.2f}%\n")

if found_regression :
    print("NEED HUMAN")

# Exit with error if regression found
sys.exit(1 if found_regression else 0)

