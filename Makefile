# Convenience wrapper around the CMake build of the CTW profile.
#
#   make            configure (first time) and build ctw_boot
#   make run        build, then run from the repo root (the game and configs/
#                   are looked up relative to it)
#   make run ARGS=<game root>     e.g. another extracted copy of the game
#   make software   run with the CPU rasterizer instead of Vulkan
#   make log        run with CTW_PRESENT_LOG=1 into captures/present.log
#   make test       build and run the framework tests
#   make clean      remove the CTW build directory
#
# Environment variables pass straight through: CTW_RENDER_SCALE=3 make run

BUILD_DIR := out/ctw
CTW_BOOT  := $(BUILD_DIR)/bin/Release/ctw_boot
JOBS      ?= $(shell nproc)

.PHONY: all build run software log test clean

all: build

$(BUILD_DIR)/CMakeCache.txt:
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Release -DPSPRECOMP_PROFILE=ctw

build: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) -j$(JOBS) --target ctw_boot

run: build
	./$(CTW_BOOT) $(ARGS)

software: build
	CTW_RENDERER=software ./$(CTW_BOOT) $(ARGS)

log: build
	@mkdir -p captures
	CTW_PRESENT_LOG=1 ./$(CTW_BOOT) $(ARGS) > captures/present.log
	@echo "wrote captures/present.log"

test: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) -j$(JOBS) --target psprecomp_tests
	./$(BUILD_DIR)/psprecomp_tests

clean:
	cmake -E rm -rf $(BUILD_DIR)
