#!/usr/bin/env python3
import sys
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

tsv = sys.argv[1] if len(sys.argv) > 1 else "log/sieve_sweep.tsv"

rows = []
with open(tsv) as f:
    next(f)
    for line in f:
        L, cands, density = line.strip().split("\t")
        L = int(L)
        if L > 0:
            rows.append((L, int(cands), float(density)))

N       = 100_000_000
sqrt_N  = int(N ** 0.5)
Ls      = [r[0] for r in rows]
cands   = [r[1] for r in rows]

fig, ax = plt.subplots(figsize=(9, 5))

ax.loglog(Ls, cands, "o-", markersize=6, color="steelblue", label="candidates")

# sqrt(N) annotation
ax.axvline(sqrt_N, color="tomato", linestyle="--", linewidth=1.2, label=f"√N = {sqrt_N:,}")
ax.annotate(f"√N = {sqrt_N:,}\nsaturation", xy=(sqrt_N, 3e6),
            xytext=(sqrt_N * 2.5, 6e6),
            arrowprops=dict(arrowstyle="->", color="tomato"),
            color="tomato", fontsize=9)

ax.set_xlabel("Sieve limit (L)")
ax.set_ylabel("Candidates remaining")
ax.set_title(f"A061955 — Candidates vs Sieve Limit  (N = {N:,})")
ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x/1e6:.1f}M"))
ax.legend()
ax.grid(True, which="both", alpha=0.25)

plt.tight_layout()
out = tsv.replace(".tsv", ".png")
plt.savefig(out, dpi=150)
print(f"Saved: {out}")
