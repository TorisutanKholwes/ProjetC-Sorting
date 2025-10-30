.PHONY: help build clean run rebuild sdl leaks install

.DEFAULT_GOAL := help

UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

BUILD_DIR := build
INSTALL_DIR := libs

APP_NAME := SDLBase

CMAKE := cmake
CMAKE_BUILD_TYPE := Release

ifeq ($(UNAME_S),Linux)
    EXECUTABLE := $(BUILD_DIR)/$(APP_NAME)
    LEAK_TOOL := valgrind
    LEAK_TOOL_ARGS := --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
    SDL_SCRIPT := bash install_libs.sh
else ifeq ($(UNAME_S),Darwin)
    EXECUTABLE := $(BUILD_DIR)/$(APP_NAME)
    LEAK_TOOL := leaks
    LEAK_TOOL_ARGS := --atExit --
    SDL_SCRIPT := bash install_libs.sh
else
    EXECUTABLE := $(BUILD_DIR)/Release/$(APP_NAME).exe
    LEAK_TOOL := echo "Memory leak detection not available on Windows. Please use Visual Studio's diagnostic tools."
    LEAK_TOOL_ARGS :=
    SDL_SCRIPT := powershell -ExecutionPolicy Bypass -File install_libs.ps1
endif

COLOR_RESET := \033[0m
COLOR_BOLD := \033[1m
COLOR_GREEN := \033[32m
COLOR_YELLOW := \033[33m
COLOR_BLUE := \033[34m

help:
	@echo "$(COLOR_BOLD)SDL_BasePrivate Makefile$(COLOR_RESET)"
	@echo "Available targets:"
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  $(COLOR_GREEN)%-15s$(COLOR_RESET) %s\n", $$1, $$2}'
	@echo ""
	@echo "$(COLOR_BLUE)Operating System Detected: $(UNAME_S)$(COLOR_RESET)"

build:
	@echo "$(COLOR_BOLD)Building $(APP_NAME)...$(COLOR_RESET)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) ..
	@cd $(BUILD_DIR) && $(CMAKE) --build . --config $(CMAKE_BUILD_TYPE)
	@echo "$(COLOR_GREEN)Build completed successfully!$(COLOR_RESET)"
	@echo "Executable: $(EXECUTABLE)"

clean:
	@echo "$(COLOR_YELLOW)Cleaning build artifacts...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(COLOR_GREEN)Clean completed!$(COLOR_RESET)"

run: build
	@echo "$(COLOR_BOLD)Running $(APP_NAME)...$(COLOR_RESET)"
	@./$(EXECUTABLE)

rebuild: clean build

sdl:
	@echo "$(COLOR_BOLD)Installing SDL3 and dependencies...$(COLOR_RESET)"
	@echo "Installation directory: $(INSTALL_DIR)"
	@$(SDL_SCRIPT) $(INSTALL_DIR)
	@echo "$(COLOR_GREEN)SDL3 installation completed!$(COLOR_RESET)"

leaks: build
	@echo "$(COLOR_BOLD)Running $(APP_NAME) with memory leak detection...$(COLOR_RESET)"
ifeq ($(UNAME_S),Linux)
	@if ! command -v $(LEAK_TOOL) > /dev/null 2>&1; then \
		echo "$(COLOR_YELLOW)Warning: valgrind not found. Installing...$(COLOR_RESET)"; \
		echo "Please run: sudo apt-get install valgrind"; \
		echo "Or on other distros: sudo yum install valgrind / sudo pacman -S valgrind"; \
		exit 1; \
	fi
	$(LEAK_TOOL) $(LEAK_TOOL_ARGS) ./$(EXECUTABLE) > leak_report.txt 2>&1
	@echo "$(COLOR_GREEN)Leak report saved to leak_report.txt$(COLOR_RESET)"
else ifeq ($(UNAME_S),Darwin)
	@if ! command -v $(LEAK_TOOL) > /dev/null 2>&1; then \
		echo "$(COLOR_YELLOW)Warning: leaks tool not found. It should be available with Xcode Command Line Tools.$(COLOR_RESET)"; \
		exit 1; \
	fi
	$(LEAK_TOOL) $(LEAK_TOOL_ARGS) ./$(EXECUTABLE) > leak_report.txt 2>&1
	@echo "$(COLOR_GREEN)Leak report saved to leak_report.txt$(COLOR_RESET)"
else
	@$(LEAK_TOOL)
endif

install: sdl build

configure:
	@echo "$(COLOR_BOLD)Configuring CMake...$(COLOR_RESET)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) ..

reconfigure:
	@echo "$(COLOR_BOLD)Reconfiguring CMake...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)/CMakeCache.txt $(BUILD_DIR)/CMakeFiles
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) ..

clean-all: clean
	@echo "$(COLOR_YELLOW)Cleaning all artifacts including SDL installation...$(COLOR_RESET)"
	@rm -rf $(INSTALL_DIR)
	@echo "$(COLOR_GREEN)All clean completed!$(COLOR_RESET)"