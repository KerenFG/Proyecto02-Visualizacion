# Compilers
CROSS_LINUX_ARM := aarch64-linux-musl-g++
CROSS_LINUX_INTEL := x86_64-linux-musl-g++
CROSS_WIN := x86_64-w64-mingw32-g++
CROSS_MAC_INTEL := g++
CROSS_MAC_ARM := g++

# Common flags
CXXFLAGS_COMMON := -std=c++17 -Os -fno-ident -fno-asynchronous-unwind-tables
CXXFLAGS_MAC := -std=c++17 -Os -fno-ident -fno-asynchronous-unwind-tables -Wall -Wextra
CXXFLAGS_WIN := -std=c++17 -static -Os -s -ffunction-sections -fdata-sections -fno-ident -static-libgcc -static-libstdc++
CXXFLAGS_WIN_SHARED := -std=c++17 -shared -Os -x c++ -static -static-libgcc -static-libstdc++ -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-ident
CXXFLAGS_LINUX_SHARED := -std=c++17 -shared -Os -fPIC -fno-ident -fno-asynchronous-unwind-tables

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin
DIST_DIR := dist
MODULES_DIR := src/modules
READERS_DIR := src/readers

# Main source files
MAIN_SRC := $(SRC_DIR)/injax.cpp

# Modules
MODULES := query chart
MODULES_SRC := $(addprefix $(MODULES_DIR)/, $(addsuffix -module.cpp, $(MODULES)))

# Readers
READERS := yaml csv xml ini
READERS_SRC := $(addprefix $(READERS_DIR)/, $(addsuffix -reader.cpp, $(READERS)))

# Output directories
LINUX_ARM_DIR := $(BIN_DIR)/linuxArm
LINUX_INTEL_DIR := $(BIN_DIR)/linuxIntel
MAC_ARM_DIR := $(BIN_DIR)/macArm
MAC_INTEL_DIR := $(BIN_DIR)/macIntel
WIN_DIR := $(BIN_DIR)/win

# Distribution directories
DIST_LINUX_ARM := $(DIST_DIR)/injax-linux-arm
DIST_LINUX_INTEL := $(DIST_DIR)/injax-linux-intel
DIST_MAC_ARM := $(DIST_DIR)/injax-mac-arm
DIST_MAC_INTEL := $(DIST_DIR)/injax-mac-intel
DIST_WIN := $(DIST_DIR)/injax-windows-x64

# Module output files
MODULES_LINUX_ARM := $(addprefix $(LINUX_ARM_DIR)/modules/lib, $(addsuffix .so, $(MODULES)))
MODULES_LINUX_INTEL := $(addprefix $(LINUX_INTEL_DIR)/modules/lib, $(addsuffix .so, $(MODULES)))
MODULES_MAC_ARM := $(addprefix $(MAC_ARM_DIR)/modules/lib, $(addsuffix .dylib, $(MODULES)))
MODULES_MAC_INTEL := $(addprefix $(MAC_INTEL_DIR)/modules/lib, $(addsuffix .dylib, $(MODULES)))
MODULES_WIN := $(addprefix $(WIN_DIR)/modules/lib, $(addsuffix .dll, $(MODULES)))

# Reader output files
READERS_LINUX_ARM := $(addprefix $(LINUX_ARM_DIR)/readers/lib, $(addsuffix .so, $(READERS)))
READERS_LINUX_INTEL := $(addprefix $(LINUX_INTEL_DIR)/readers/lib, $(addsuffix .so, $(READERS)))
READERS_MAC_ARM := $(addprefix $(MAC_ARM_DIR)/readers/lib, $(addsuffix .dylib, $(READERS)))
READERS_MAC_INTEL := $(addprefix $(MAC_INTEL_DIR)/readers/lib, $(addsuffix .dylib, $(READERS)))
READERS_WIN := $(addprefix $(WIN_DIR)/readers/lib, $(addsuffix .dll, $(READERS)))

# All directories to create
DIRS := $(LINUX_ARM_DIR) $(LINUX_ARM_DIR)/modules $(LINUX_ARM_DIR)/readers \
        $(LINUX_INTEL_DIR) $(LINUX_INTEL_DIR)/modules $(LINUX_INTEL_DIR)/readers \
        $(MAC_ARM_DIR) $(MAC_ARM_DIR)/modules $(MAC_ARM_DIR)/readers \
        $(MAC_INTEL_DIR) $(MAC_INTEL_DIR)/modules $(MAC_INTEL_DIR)/readers \
        $(WIN_DIR) $(WIN_DIR)/modules $(WIN_DIR)/readers \
        $(DIST_DIR) $(DIST_LINUX_ARM) $(DIST_LINUX_INTEL) $(DIST_MAC_ARM) $(DIST_MAC_INTEL) $(DIST_WIN)

.PHONY: all clean $(TARGETS) modules readers dist dist-all

# Main targets
TARGETS := linuxArm linuxIntel macArm macIntel win

directories: $(DIRS)
all: directories $(TARGETS) modules readers

# Create directories
$(DIRS):
	mkdir -p $@

# ============================================================================
# BINARIES (Main executable)
# ============================================================================

# Linux ARM
linuxArm: $(LINUX_ARM_DIR)/injax | $(LINUX_ARM_DIR)

$(LINUX_ARM_DIR)/injax: $(MAIN_SRC)
	$(CROSS_LINUX_ARM) -static $(CXXFLAGS_COMMON) -I$(INCLUDE_DIR) $< -o $@

# Linux Intel
linuxIntel: $(LINUX_INTEL_DIR)/injax | $(LINUX_INTEL_DIR)

$(LINUX_INTEL_DIR)/injax: $(MAIN_SRC)
	$(CROSS_LINUX_INTEL) -static $(CXXFLAGS_COMMON) -I$(INCLUDE_DIR) $< -o $@

# Mac ARM
macArm: $(MAC_ARM_DIR)/injax | $(MAC_ARM_DIR)

$(MAC_ARM_DIR)/injax: $(MAIN_SRC)
	$(CROSS_MAC_ARM) $(CXXFLAGS_MAC) -O3 -I$(INCLUDE_DIR) $< -o $@

# Mac Intel
macIntel: $(MAC_INTEL_DIR)/injax | $(MAC_INTEL_DIR)

$(MAC_INTEL_DIR)/injax: $(MAIN_SRC)
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin $(CXXFLAGS_MAC) -I$(INCLUDE_DIR) $< -o $@

# Windows
win: $(WIN_DIR)/injax.exe | $(WIN_DIR)

$(WIN_DIR)/injax.exe: $(MAIN_SRC)
	$(CROSS_WIN) $(CXXFLAGS_WIN) -Wl,--gc-sections -I$(INCLUDE_DIR) $< -o $@

# ============================================================================
# MODULES
# ============================================================================

# Linux ARM modules
modules-linux-arm: $(MODULES_LINUX_ARM) | $(LINUX_ARM_DIR)/modules

$(LINUX_ARM_DIR)/modules/libquery.so: $(MODULES_DIR)/query-module.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_ARM_DIR)/modules/libchart.so: $(MODULES_DIR)/chart-module.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

# Linux Intel modules
modules-linux-intel: $(MODULES_LINUX_INTEL) | $(LINUX_INTEL_DIR)/modules

$(LINUX_INTEL_DIR)/modules/libquery.so: $(MODULES_DIR)/query-module.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_INTEL_DIR)/modules/libchart.so: $(MODULES_DIR)/chart-module.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

# Mac ARM modules
modules-mac-arm: $(MODULES_MAC_ARM) | $(MAC_ARM_DIR)/modules

$(MAC_ARM_DIR)/modules/libquery.dylib: $(MODULES_DIR)/query-module.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_ARM_DIR)/modules/libchart.dylib: $(MODULES_DIR)/chart-module.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

# Mac Intel modules
modules-mac-intel: $(MODULES_MAC_INTEL) | $(MAC_INTEL_DIR)/modules

$(MAC_INTEL_DIR)/modules/libquery.dylib: $(MODULES_DIR)/query-module.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_INTEL_DIR)/modules/libchart.dylib: $(MODULES_DIR)/chart-module.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

# Windows modules
modules-win: $(MODULES_WIN) | $(WIN_DIR)/modules

$(WIN_DIR)/modules/libquery.dll: $(MODULES_DIR)/query-module.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(WIN_DIR)/modules/libchart.dll: $(MODULES_DIR)/chart-module.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@

# ============================================================================
# READERS
# ============================================================================

# Linux ARM readers
readers-linux-arm: $(READERS_LINUX_ARM) | $(LINUX_ARM_DIR)/readers

$(LINUX_ARM_DIR)/readers/libyaml.so: $(READERS_DIR)/yaml-reader.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_ARM_DIR)/readers/libcsv.so: $(READERS_DIR)/csv-reader.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_ARM_DIR)/readers/libxml.so: $(READERS_DIR)/xml-reader.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_ARM_DIR)/readers/libini.so: $(READERS_DIR)/ini-reader.cpp
	$(CROSS_LINUX_ARM) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

# Linux Intel readers
readers-linux-intel: $(READERS_LINUX_INTEL) | $(LINUX_INTEL_DIR)/readers

$(LINUX_INTEL_DIR)/readers/libyaml.so: $(READERS_DIR)/yaml-reader.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_INTEL_DIR)/readers/libcsv.so: $(READERS_DIR)/csv-reader.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_INTEL_DIR)/readers/libxml.so: $(READERS_DIR)/xml-reader.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

$(LINUX_INTEL_DIR)/readers/libini.so: $(READERS_DIR)/ini-reader.cpp
	$(CROSS_LINUX_INTEL) $(CXXFLAGS_LINUX_SHARED) -I$(INCLUDE_DIR) $< -o $@

# Mac ARM readers
readers-mac-arm: $(READERS_MAC_ARM) | $(MAC_ARM_DIR)/readers

$(MAC_ARM_DIR)/readers/libyaml.dylib: $(READERS_DIR)/yaml-reader.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_ARM_DIR)/readers/libcsv.dylib: $(READERS_DIR)/csv-reader.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_ARM_DIR)/readers/libxml.dylib: $(READERS_DIR)/xml-reader.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_ARM_DIR)/readers/libini.dylib: $(READERS_DIR)/ini-reader.cpp
	$(CROSS_MAC_ARM) -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

# Mac Intel readers
readers-mac-intel: $(READERS_MAC_INTEL) | $(MAC_INTEL_DIR)/readers

$(MAC_INTEL_DIR)/readers/libyaml.dylib: $(READERS_DIR)/yaml-reader.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_INTEL_DIR)/readers/libcsv.dylib: $(READERS_DIR)/csv-reader.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_INTEL_DIR)/readers/libxml.dylib: $(READERS_DIR)/xml-reader.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

$(MAC_INTEL_DIR)/readers/libini.dylib: $(READERS_DIR)/ini-reader.cpp
	$(CROSS_MAC_INTEL) --target=x86_64-apple-darwin -std=c++17 -dynamiclib -o $@ $< -O3 -Wall -Wextra -I$(INCLUDE_DIR)

# Windows readers
readers-win: $(READERS_WIN) | $(WIN_DIR)

$(WIN_DIR)/readers/libyaml.dll: $(READERS_DIR)/yaml-reader.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@ -Wl,--out-implib,$(WIN_DIR)/libyaml.a

$(WIN_DIR)/readers/libcsv.dll: $(READERS_DIR)/csv-reader.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@ -Wl,--out-implib,$(WIN_DIR)/libcsv.a

$(WIN_DIR)/readers/libxml.dll: $(READERS_DIR)/xml-reader.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@ -Wl,--out-implib,$(WIN_DIR)/libxml.a

$(WIN_DIR)/readers/libini.dll: $(READERS_DIR)/ini-reader.cpp
	$(CROSS_WIN) $(CXXFLAGS_WIN_SHARED) -I$(INCLUDE_DIR) $< -o $@ -Wl,--out-implib,$(WIN_DIR)/libini.a

# ============================================================================
# AGGREGATE TARGETS
# ============================================================================

modules: modules-linux-arm modules-linux-intel modules-mac-arm modules-mac-intel modules-win

readers: readers-linux-arm readers-linux-intel readers-mac-arm readers-mac-intel readers-win

# ============================================================================
# DISTRIBUTION PACKAGES
# ============================================================================

# Linux ARM package
dist-linux-arm: linuxArm modules-linux-arm readers-linux-arm $(DIST_LINUX_ARM)
	@echo "Creating Linux ARM distribution package..."
	@rm -rf $(DIST_LINUX_ARM)/*
	@cp $(LINUX_ARM_DIR)/injax $(DIST_LINUX_ARM)/
	@cp -r $(LINUX_ARM_DIR)/modules $(DIST_LINUX_ARM)/
	@cp -r $(LINUX_ARM_DIR)/readers $(DIST_LINUX_ARM)/
	@echo "#!/bin/bash" > $(DIST_LINUX_ARM)/run.sh
	@echo "./injax \"\$$@\"" >> $(DIST_LINUX_ARM)/run.sh
	@chmod +x $(DIST_LINUX_ARM)/run.sh
	@chmod +x $(DIST_LINUX_ARM)/injax
	@cd $(DIST_DIR) && zip -r injax-linux-arm.zip injax-linux-arm/
	@echo "Created: $(DIST_DIR)/injax-linux-arm.zip"

# Linux Intel package
dist-linux-intel: linuxIntel modules-linux-intel readers-linux-intel $(DIST_LINUX_INTEL)
	@echo "Creating Linux Intel distribution package..."
	@rm -rf $(DIST_LINUX_INTEL)/*
	@cp $(LINUX_INTEL_DIR)/injax $(DIST_LINUX_INTEL)/
	@cp -r $(LINUX_INTEL_DIR)/modules $(DIST_LINUX_INTEL)/
	@cp -r $(LINUX_INTEL_DIR)/readers $(DIST_LINUX_INTEL)/
	@echo "#!/bin/bash" > $(DIST_LINUX_INTEL)/run.sh
	@echo "./injax \"\$$@\"" >> $(DIST_LINUX_INTEL)/run.sh
	@chmod +x $(DIST_LINUX_INTEL)/run.sh
	@chmod +x $(DIST_LINUX_INTEL)/injax
	@cd $(DIST_DIR) && zip -r injax-linux-intel.zip injax-linux-intel/
	@echo "Created: $(DIST_DIR)/injax-linux-intel.zip"

# Mac ARM package
dist-mac-arm: macArm modules-mac-arm readers-mac-arm $(DIST_MAC_ARM)
	@echo "Creating Mac ARM distribution package..."
	@rm -rf $(DIST_MAC_ARM)/*
	@cp $(MAC_ARM_DIR)/injax $(DIST_MAC_ARM)/
	@cp -r $(MAC_ARM_DIR)/modules $(DIST_MAC_ARM)/
	@cp -r $(MAC_ARM_DIR)/readers $(DIST_MAC_ARM)/
	@echo "#!/bin/bash" > $(DIST_MAC_ARM)/run.sh
	@echo "./injax \"\$$@\"" >> $(DIST_MAC_ARM)/run.sh
	@chmod +x $(DIST_MAC_ARM)/run.sh
	@chmod +x $(DIST_MAC_ARM)/injax
	@cd $(DIST_DIR) && zip -r injax-mac-arm.zip injax-mac-arm/
	@echo "Created: $(DIST_DIR)/injax-mac-arm.zip"

# Mac Intel package
dist-mac-intel: macIntel modules-mac-intel readers-mac-intel $(DIST_MAC_INTEL)
	@echo "Creating Mac Intel distribution package..."
	@rm -rf $(DIST_MAC_INTEL)/*
	@cp $(MAC_INTEL_DIR)/injax $(DIST_MAC_INTEL)/
	@cp -r $(MAC_INTEL_DIR)/modules $(DIST_MAC_INTEL)/
	@cp -r $(MAC_INTEL_DIR)/readers $(DIST_MAC_INTEL)/
	@echo "#!/bin/bash" > $(DIST_MAC_INTEL)/run.sh
	@echo "./injax \"\$$@\"" >> $(DIST_MAC_INTEL)/run.sh
	@chmod +x $(DIST_MAC_INTEL)/run.sh
	@chmod +x $(DIST_MAC_INTEL)/injax
	@cd $(DIST_DIR) && zip -r injax-mac-intel.zip injax-mac-intel/
	@echo "Created: $(DIST_DIR)/injax-mac-intel.zip"

# Windows package
dist-win: win modules-win readers-win $(DIST_WIN)
	@echo "Creating Windows distribution package..."
	@rm -rf $(DIST_WIN)/*
	@cp $(WIN_DIR)/injax.exe $(DIST_WIN)/
	@cp -r $(WIN_DIR)/modules $(DIST_WIN)/
	@cp -r $(WIN_DIR)/readers $(DIST_WIN)/
	@echo "@echo off" > $(DIST_WIN)/run.bat
	@echo "injax.exe %*" >> $(DIST_WIN)/run.bat
	@cd $(DIST_DIR) && zip -r injax-windows-x64.zip injax-windows-x64/
	@echo "Created: $(DIST_DIR)/injax-windows-x64.zip"

# Create all distribution packages
dist-all: dist-linux-arm dist-linux-intel dist-mac-arm dist-mac-intel dist-win
	@echo ""
	@echo "========================================="
	@echo "All distribution packages created in $(DIST_DIR)/"
	@echo "========================================="
	@ls -lh $(DIST_DIR)/*.zip

# Create a package with all platforms combined
dist-combined: dist-all
	@echo "Creating combined package..."
	@cd $(DIST_DIR) && zip -r injax-all-platforms.zip injax-linux-arm/ injax-linux-intel/ injax-mac-arm/ injax-mac-intel/ injax-windows-x64/
	@echo "Created: $(DIST_DIR)/injax-all-platforms.zip"

# ============================================================================
# CLEAN TARGETS
# ============================================================================

clean:
	rm -rf $(BIN_DIR) $(DIST_DIR)

clean-linux:
	rm -rf $(LINUX_ARM_DIR) $(LINUX_INTEL_DIR)

clean-mac:
	rm -rf $(MAC_ARM_DIR) $(MAC_INTEL_DIR)

clean-win:
	rm -rf $(WIN_DIR)

clean-modules:
	rm -f $(LINUX_ARM_DIR)/modules/*.so $(LINUX_INTEL_DIR)/modules/*.so
	rm -f $(MAC_ARM_DIR)/modules/*.dylib $(MAC_INTEL_DIR)/modules/*.dylib
	rm -f $(WIN_DIR)/modules/*.dll

clean-readers:
	rm -f $(LINUX_ARM_DIR)/readers/*.so $(LINUX_INTEL_DIR)/readers/*.so
	rm -f $(MAC_ARM_DIR)/readers/*.dylib $(MAC_INTEL_DIR)/readers/*.dylib
	rm -f $(WIN_DIR)/readers/lib*.dll $(WIN_DIR)/readers/lib*.a

clean-dist:
	rm -rf $(DIST_DIR)

# ============================================================================
# HELP
# ============================================================================

help:
	@echo "Available targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  all           - Build everything (binaries, modules and readers for all platforms)"
	@echo "  linuxArm      - Build for Linux ARM"
	@echo "  linuxIntel    - Build for Linux Intel"
	@echo "  macArm        - Build for Mac ARM"
	@echo "  macIntel      - Build for Mac Intel"
	@echo "  win           - Build for Windows"
	@echo "  modules       - Build modules for all platforms"
	@echo "  readers       - Build readers for all platforms"
	@echo ""
	@echo "Distribution packages (ZIP):"
	@echo "  dist-linux-arm    - Create Linux ARM distribution package"
	@echo "  dist-linux-intel  - Create Linux Intel distribution package"
	@echo "  dist-mac-arm      - Create Mac ARM distribution package"
	@echo "  dist-mac-intel    - Create Mac Intel distribution package"
	@echo "  dist-win          - Create Windows distribution package"
	@echo "  dist-all          - Create all distribution packages"
	@echo "  dist-combined     - Create a single ZIP with all platforms"
	@echo ""
	@echo "Clean targets:"
	@echo "  clean         - Remove entire bin and dist directories"
	@echo "  clean-linux   - Remove Linux binaries"
	@echo "  clean-mac     - Remove Mac binaries"
	@echo "  clean-win     - Remove Windows binaries"
	@echo "  clean-modules - Remove all modules"
	@echo "  clean-readers - Remove all readers"
	@echo "  clean-dist    - Remove distribution packages only"
	@echo ""
	@echo "Help:"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make dist-all          - Build everything and create all distribution packages"
	@echo "  make dist-mac-arm      - Build and package only for Mac ARM"
	@echo "  make dist-combined     - Create a single ZIP with all platforms"

# Default target
.DEFAULT_GOAL := help