import json
import os

import numpy as np
import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "price_battery_scatter.json")

# RColorBrewer Set1, 6 colors — assigned to top-6 brands in descending order
SET1_6 = ["#E41A1C", "#377EB8", "#4DAF4A", "#984EA3", "#FF7F00", "#A65628"]

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name", "Launched Price (USA)",
         "Battery Capacity", "Screen Size", "RAM"]].copy()
df.columns = ["company", "model", "price_raw", "battery_raw", "screen_raw", "ram_raw"]

df["price_usd"] = (
    df["price_raw"].str.strip().str.removeprefix("USD ")
    .str.replace(",", "", regex=False)
    .pipe(pd.to_numeric, errors="coerce")
)
df["battery_mah"] = (
    df["battery_raw"].str.strip()
    .str.replace(",", "", regex=False).str.replace("mAh", "", regex=False)
    .str.strip().pipe(pd.to_numeric, errors="coerce")
)
df["screen_in"] = (
    df["screen_raw"].str.strip()
    .str.replace(" inches", "", regex=False)
    .pipe(pd.to_numeric, errors="coerce")
)
df["ram_gb"] = (
    df["ram_raw"].str.strip().str.replace("GB", "", regex=False)
    .str.strip().pipe(pd.to_numeric, errors="coerce")
)

df = df.dropna(subset=["price_usd", "battery_mah", "screen_in", "ram_gb"])
df = df[df["screen_in"] > 0]
df = df[df["price_usd"] <= 5_000]

# Dedup: exact rows, then name-duplicates keeping lower price
df = df.drop_duplicates()
df = (df.sort_values("price_usd")
        .drop_duplicates(subset=["company", "model"], keep="first"))

# --- Top 6 brands -------------------------------------------------------

top6 = (df.groupby("company").size()
          .sort_values(ascending=False)
          .head(6).index.tolist())
color_map = dict(zip(top6, SET1_6))

df6 = df[df["company"].isin(top6)].copy()
print(f"[clean] {len(df6)} points across top-6 brands")
for b in top6:
    print(f"  {b}: {len(df6[df6['company']==b])}")

# --- Size mapping: screen_in → radius [2, 8] ----------------------------

s_min = float(df6["screen_in"].min())
s_max = float(df6["screen_in"].max())

def screen_to_radius(s):
    if s_max == s_min:
        return 4.0
    return round(2.0 + (s - s_min) / (s_max - s_min) * (8.0 - 2.0), 2)

# --- Build records -------------------------------------------------------

records = [
    {
        "brand":       row["company"],
        "model":       row["model"],
        "battery_mah": int(row["battery_mah"]),
        "price_usd":   round(float(row["price_usd"]), 2),
        "screen_in":   round(float(row["screen_in"]), 1),
        "ram_gb":      int(row["ram_gb"]),
        "color":       color_map[row["company"]],
        "radius":      screen_to_radius(row["screen_in"]),
    }
    for _, row in df6.iterrows()
]

# --- Output JSON --------------------------------------------------------

payload = {
    "data": records,
    "bat_domain_min":   round(float(df6["battery_mah"].min()) * 0.95, 1),
    "bat_domain_max":   round(float(df6["battery_mah"].max()) * 1.02, 1),
    "price_domain_max": round(float(df6["price_usd"].max())   * 1.08, 1),
    "legend": [{"brand": b, "color": color_map[b]} for b in top6],
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] {len(records)} points — saved to {OUT_PATH}")
