#!/usr/bin/env bash
# Orquestador principal: pipeline → JSON → SVG para todas las gráficas.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INJAX_DIR="$ROOT/InjaX"
PIPELINES_DIR="$ROOT/pipelines"
DATA_DIR="$ROOT/data/processed"
TEMPLATES_DIR="$ROOT/templates"
OUTPUTS_DIR="$ROOT/outputs"

# ── 1. Compilar InjaX si el binario no existe o la fuente es más nueva ──────

cd "$INJAX_DIR"

if [[ ! -f injax ]] || [[ src/injax.cpp -nt injax ]]; then
    echo "[build] Compilando injax CLI..."
    g++ -std=c++17 -Iinclude src/injax.cpp -o injax
fi

mkdir -p modules

if [[ ! -f modules/libchart.so ]] || [[ src/modules/chart-module.cpp -nt modules/libchart.so ]]; then
    echo "[build] Compilando módulo chart..."
    g++ -std=c++17 -shared -fPIC -Os -Iinclude src/modules/chart-module.cpp -o modules/libchart.so
fi

cd "$ROOT"

# ── 2. Preparar directorios de salida ───────────────────────────────────────

mkdir -p "$DATA_DIR" "$OUTPUTS_DIR"

# ── 3. Iterar sobre cada pipeline ───────────────────────────────────────────

RENDERED=0
SKIPPED=0

for script in "$PIPELINES_DIR"/*.py; do
    CHART="$(basename "$script" .py)"
    JSON="$DATA_DIR/$CHART.json"
    TEMPLATE="$TEMPLATES_DIR/$CHART.inja"
    SVG="$OUTPUTS_DIR/$CHART.svg"

    echo ""
    echo "▸ $CHART"

    # 3a. Generar JSON desde el pipeline
    python3 "$script"

    # 3b. Renderizar SVG si existe la plantilla
    if [[ ! -f "$TEMPLATE" ]]; then
        echo "  [skip] plantilla no encontrada: templates/$CHART.inja"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    "$INJAX_DIR/injax" "$JSON" "$TEMPLATE" "$SVG"
    echo "  [ok]   outputs/$CHART.svg"
    RENDERED=$((RENDERED + 1))
done

echo ""
echo "────────────────────────────────────"
echo "Listo: $RENDERED SVG(s) generados, $SKIPPED omitidos (sin plantilla)."
echo "────────────────────────────────────"
