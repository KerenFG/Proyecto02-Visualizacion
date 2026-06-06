import json
import os

import pandas as pd

CSV_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "MobilesDataset2025.csv")
OUT_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "processed",
                        "top10_brands_by_models.json")

# --- Load & clean -------------------------------------------------------

df = pd.read_csv(CSV_PATH, encoding="latin-1")

df = df[["Company Name", "Model Name"]].copy()
df.columns = ["company", "model"]

# Drop exact duplicates before counting
df = df.drop_duplicates()

# --- Aggregate top 10 ---------------------------------------------------

counts = (
    df.groupby("company")["model"]
    .count()
    .rename("count")
    .sort_values(ascending=False)
    .head(10)
    .reset_index()
)

# Sort ascending for the bar chart (lowest count first = top of horizontal chart
# mirrors reorder(Brand, Count) + coord_flip() in ggplot)
counts = counts.sort_values("count", ascending=True).reset_index(drop=True)

# --- Color gradient (#90CAF9 low → #1565C0 high) ------------------------

LOW  = (144, 202, 249)   # #90CAF9
HIGH = ( 21, 101, 192)   # #1565C0

count_min = int(counts["count"].min())
count_max = int(counts["count"].max())
span = count_max - count_min if count_max != count_min else 1

def interpolate_color(value):
    t = (value - count_min) / span
    r = round(LOW[0] + t * (HIGH[0] - LOW[0]))
    g = round(LOW[1] + t * (HIGH[1] - LOW[1]))
    b = round(LOW[2] + t * (HIGH[2] - LOW[2]))
    return f"#{r:02X}{g:02X}{b:02X}"

# --- Build records -------------------------------------------------------

brands = [
    {
        "brand": row["company"],
        "count": int(row["count"]),
        "color": interpolate_color(row["count"]),
    }
    for _, row in counts.iterrows()
]

# --- Output JSON --------------------------------------------------------

payload = {
    "data": brands,
    "total_brands": int(len(counts)),
    "count_min": count_min,
    "count_max": count_max,
}

os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
with open(OUT_PATH, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2, ensure_ascii=False)

print(f"[OK] top {len(brands)} brands — max={count_max} models — saved to {OUT_PATH}")
