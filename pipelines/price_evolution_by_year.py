import json
import os

import numpy as np
import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "price_evolution_by_year.json")

# RColorBrewer Set2, 7 colors — one per year 2019→2025
SET2_7 = ["#66C2A5", "#FC8D62", "#8DA0CB", "#E78AC3", "#A6D854", "#FFD92F", "#E5C494"]

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name", "Launched Year", "Launched Price (USA)"]].copy()
df.columns = ["company", "model", "year_raw", "price_raw"]

df["price_usd"] = (
    df["price_raw"]
    .str.strip()
    .str.removeprefix("USD ")
    .str.replace(",", "", regex=False)
    .pipe(pd.to_numeric, errors="coerce")
)

df["year"] = pd.to_numeric(df["year_raw"], errors="coerce")

df = df.dropna(subset=["price_usd", "year"])

# Filter: year >= 2019, price <= 5000 (consistent with other pipelines)
df = df[df["year"] >= 2019]
df = df[df["price_usd"] <= 5_000]

# Dedup: exact rows, then name-duplicates keeping lower price
df = df.drop_duplicates()
df = (df.sort_values("price_usd")
        .drop_duplicates(subset=["company", "model"], keep="first"))

print(f"[clean] {len(df)} rows after filtering (year >= 2019 + price + dedup)")

# --- Quartile stats per year --------------------------------------------

years = sorted(df["year"].unique().astype(int))
color_map = dict(zip(years, SET2_7))

groups = []
for yr in years:
    sub = df[df["year"] == yr]["price_usd"].values
    q1  = float(np.percentile(sub, 25))
    q2  = float(np.percentile(sub, 50))
    q3  = float(np.percentile(sub, 75))
    groups.append({
        "year":  int(yr),
        "color": color_map[int(yr)],
        "count": int(len(sub)),
        "q1":    round(q1, 2),
        "q2":    round(q2, 2),
        "q3":    round(q3, 2),
    })
    print(f"  {yr}: n={len(sub):3}  Q1={q1:.0f}  Q2={q2:.0f}  Q3={q3:.0f}")

# --- Output JSON --------------------------------------------------------

payload = {
    "data":      groups,
    "price_max": round(float(df["price_usd"].max()), 2),
    "year_min":  int(years[0]),
    "year_max":  int(years[-1]),
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] {len(groups)} years — saved to {OUT_PATH}")
