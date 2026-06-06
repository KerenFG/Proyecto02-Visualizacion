import json
import os

import numpy as np
import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "price_distribution_on_release_usd.json")

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name", "Launched Year", "Launched Price (USA)"]].copy()
df.columns = ["company", "model", "year", "price_raw"]

df["price_usd"] = (
    df["price_raw"]
    .str.strip()
    .str.removeprefix("USD ")
    .str.replace(",", "", regex=False)
    .pipe(pd.to_numeric, errors="coerce")
)

df["year"] = pd.to_numeric(df["year"], errors="coerce").astype("Int64")

df = df.dropna(subset=["price_usd", "year"])

# --- Data cleaning ------------------------------------------------------

rows_before = len(df)

# 1. Drop exact duplicates
df = df.drop_duplicates()

# 2. For name-duplicates with conflicting prices, keep the lower price
df = df.sort_values("price_usd").drop_duplicates(subset=["company", "model"], keep="first")

# 3. Remove price outliers above $5,000 (data entry errors, e.g. Nokia T21 at $39,622)
OUTLIER_THRESHOLD = 5_000
outliers = df[df["price_usd"] > OUTLIER_THRESHOLD][["company", "model", "price_usd"]]
df = df[df["price_usd"] <= OUTLIER_THRESHOLD]

rows_removed = rows_before - len(df)
print(f"[clean] removed {rows_removed} rows ({len(outliers)} outliers, rest duplicates)")
if not outliers.empty:
    print(outliers.to_string(index=False))

# --- Stats --------------------------------------------------------------

median = float(np.median(df["price_usd"]))

# --- Histogram bins (40 bins, same as R code) ---------------------------

counts, edges = np.histogram(df["price_usd"], bins=40)

bins = [
    {
        "bin_index": int(i),
        "x0": round(float(edges[i]), 2),
        "x1": round(float(edges[i + 1]), 2),
        "x_mid": round(float((edges[i] + edges[i + 1]) / 2), 2),
        "count": int(counts[i]),
    }
    for i in range(len(counts))
]

max_count = int(counts.max())

# --- Output JSON --------------------------------------------------------

payload = {
    "metadata": {
        "total_models": len(df),
        "rows_removed": rows_removed,
        "outlier_threshold": OUTLIER_THRESHOLD,
        "price_min": round(float(df["price_usd"].min()), 2),
        "price_max": round(float(df["price_usd"].max()), 2),
        "median": round(median, 2),
        "bin_width": round(float(edges[1] - edges[0]), 2),
        "max_count": max_count,
        "annotation_x": round(median + 100, 2),
        "annotation_y": 55,
    },
    "data": bins,
    "raw": df[["company", "model", "year", "price_usd"]].to_dict(orient="records"),
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] {len(df)} models — median=${median:.0f} — saved to {OUT_PATH}")
