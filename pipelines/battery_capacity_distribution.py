import json
import os

import numpy as np
import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "battery_capacity_distribution.json")

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name", "Battery Capacity"]].copy()
df.columns = ["company", "model", "battery_raw"]

df["battery_mah"] = (
    df["battery_raw"]
    .str.strip()
    .str.replace(",", "", regex=False)
    .str.replace("mAh", "", regex=False)
    .str.strip()
    .pipe(pd.to_numeric, errors="coerce")
)

df = df.dropna(subset=["battery_mah"])

# --- Data cleaning ------------------------------------------------------

rows_before = len(df)

# 1. Drop exact duplicates
df = df.drop_duplicates()

# 2. Name-duplicates with conflicting battery — keep the lower value
df = df.sort_values("battery_mah").drop_duplicates(subset=["company", "model"], keep="first")

rows_removed = rows_before - len(df)
print(f"[clean] removed {rows_removed} rows (duplicates)")

# --- Stats --------------------------------------------------------------

median = float(np.median(df["battery_mah"]))

# --- Histogram bins (35 bins, same as R code) ---------------------------

counts, edges = np.histogram(df["battery_mah"], bins=35)

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
    "data": bins,
    "total_models": int(len(df)),
    "rows_removed": int(rows_removed),
    "bat_min": round(float(df["battery_mah"].min()), 2),
    "bat_max": round(float(df["battery_mah"].max()), 2),
    "median": round(float(median), 2),
    "bin_width": round(float(edges[1] - edges[0]), 2),
    "max_count": int(max_count),
    "annotation_x": round(float(median) + 350, 2),
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] {len(df)} models — median={median:.0f} mAh — saved to {OUT_PATH}")
