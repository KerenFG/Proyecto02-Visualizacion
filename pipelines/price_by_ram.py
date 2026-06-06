import json
import os

import numpy as np
import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "price_by_ram.json")

RAM_VALUES  = [3, 4, 6, 8, 12, 16]
# RColorBrewer "Blues" palette, 6 colors light→dark
BLUES_6     = ["#C6DBEF", "#9ECAE1", "#6BAED6", "#4292C6", "#2171B5", "#084594"]
COLOR_MAP   = dict(zip(RAM_VALUES, BLUES_6))

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name", "RAM", "Launched Price (USA)"]].copy()
df.columns = ["company", "model", "ram_raw", "price_raw"]

df["ram_gb"] = (
    df["ram_raw"]
    .str.strip()
    .str.replace("GB", "", regex=False)
    .str.strip()
    .pipe(pd.to_numeric, errors="coerce")
)

df["price_usd"] = (
    df["price_raw"]
    .str.strip()
    .str.removeprefix("USD ")
    .str.replace(",", "", regex=False)
    .pipe(pd.to_numeric, errors="coerce")
)

df = df.dropna(subset=["ram_gb", "price_usd"])

# Filter to target RAM values and price outlier threshold (consistent with price pipeline)
df = df[df["ram_gb"].isin(RAM_VALUES)]
df = df[df["price_usd"] <= 5_000]

# Dedup: exact rows first, then name-duplicates keeping lower price
df = df.drop_duplicates()
df = (df.sort_values("price_usd")
        .drop_duplicates(subset=["company", "model"], keep="first"))

rows_removed_total = 930 - len(df)
print(f"[clean] {len(df)} rows after filtering (RAM + price + dedup)")

# --- Boxplot stats per RAM group ----------------------------------------

def boxplot_stats(prices: np.ndarray) -> dict:
    q1  = float(np.percentile(prices, 25))
    q2  = float(np.percentile(prices, 50))
    q3  = float(np.percentile(prices, 75))
    iqr = q3 - q1
    lf  = q1 - 1.5 * iqr
    uf  = q3 + 1.5 * iqr
    lw  = float(prices[prices >= lf].min())
    uw  = float(prices[prices <= uf].max())
    out = sorted(float(p) for p in prices if p < lf or p > uf)
    return {
        "q1":            round(q1, 2),
        "q2":            round(q2, 2),
        "q3":            round(q3, 2),
        "iqr":           round(iqr, 2),
        "lower_whisker": round(lw, 2),
        "upper_whisker": round(uw, 2),
        "outliers":      [round(o, 2) for o in out],
    }

groups = []
for ram in RAM_VALUES:
    sub = df[df["ram_gb"] == ram]["price_usd"].values
    if len(sub) == 0:
        continue
    stats = boxplot_stats(sub)
    groups.append({
        "ram":   int(ram),
        "color": COLOR_MAP[ram],
        "count": int(len(sub)),
        **stats,
    })
    print(f"  {ram:2}GB  n={len(sub):3}  "
          f"Q1={stats['q1']:.0f}  Q2={stats['q2']:.0f}  Q3={stats['q3']:.0f}  "
          f"outliers={len(stats['outliers'])}")

# --- Output JSON --------------------------------------------------------

payload = {
    "data":      groups,
    "price_max": round(float(df["price_usd"].max()), 2),
    "ram_labels": [f"{r} GB" for r in RAM_VALUES],
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] {len(groups)} RAM groups — saved to {OUT_PATH}")
