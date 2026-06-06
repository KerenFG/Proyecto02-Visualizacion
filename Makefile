SHELL  := /bin/bash
ROOT   := $(shell pwd)
INJAX  := $(ROOT)/InjaX/injax

.PHONY: all build clean chart help

# ── Targets públicos ─────────────────────────────────────────────────────────

## Genera todos los SVGs (pipeline + render)
all:
	bash automation/run_all.sh

## Compila el binario injax y el módulo chart
build:
	cd InjaX && \
	  g++ -std=c++17 -Iinclude src/injax.cpp -o injax
	cd InjaX && \
	  mkdir -p modules && \
	  g++ -std=c++17 -shared -fPIC -Os -Iinclude src/modules/chart-module.cpp -o modules/libchart.so
	@echo "[build] Compilación completada."

## Borra SVGs y JSONs procesados (las fuentes de verdad no se tocan)
clean:
	rm -f outputs/*.svg
	rm -f data/processed/*.json
	@echo "[clean] Outputs borrados."

## Genera una sola gráfica: make chart name=<chart_name>
chart:
ifndef name
	$(error Uso: make chart name=<chart_name>)
endif
	python3 pipelines/$(name).py
	$(INJAX) \
	  "$(ROOT)/data/processed/$(name).json" \
	  "$(ROOT)/templates/$(name).inja" \
	  "$(ROOT)/outputs/$(name).svg"
	@echo "[ok] outputs/$(name).svg generado."

## Muestra esta ayuda
help:
	@echo ""
	@echo "  make all              — corre todos los pipelines y genera todos los SVGs"
	@echo "  make build            — compila injax y el módulo chart"
	@echo "  make chart name=X     — pipeline + render de una sola gráfica"
	@echo "  make clean            — borra outputs/*.svg y data/processed/*.json"
	@echo ""
