# C Gaming Monorepo Makefile with Raylib
# Scalable build system for multiple games and shared libraries

# =============================================================================
# CORE CONFIGURATION
# =============================================================================

CC := clang
AR := ar
MODE ?= release
PLATFORM ?= PLATFORM_DESKTOP

# Platform detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Raylib platform configuration
ifeq ($(UNAME_S),Darwin)
  PLATFORM_OS = OSX
  RAYLIB_LIBS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
  ifeq ($(UNAME_M),arm64)
    ARM64_OPTS := -mcpu=apple-m1 -mtune=apple-m1 -arch arm64
  else
    ARM64_OPTS := -march=native
  endif
else ifeq ($(UNAME_S),Linux)
  PLATFORM_OS = LINUX
  RAYLIB_LIBS = -lGL -lm -lpthread -ldl -lrt -lX11
  ARM64_OPTS := -march=native -mtune=native
else
  PLATFORM_OS = LINUX
  RAYLIB_LIBS = -lGL -lm -lpthread -ldl -lrt -lX11
  ARM64_OPTS := -march=native
endif

# Build mode configuration
ifeq ($(MODE),debug)
  OPTS := -g3 -O0 -DDEBUG
else
  OPTS := -O3 -DNDEBUG -flto -ffast-math
endif

# Compiler flags
CFLAGS := -std=c11 -Wall -Wextra $(ARM64_OPTS) $(OPTS) -D$(PLATFORM)

# =============================================================================
# PATHS AND DISCOVERY
# =============================================================================

BUILD_DIR := build
VENDOR_DIR := vendor
RAYLIB_DIR := $(VENDOR_DIR)/raylib
RAYLIB_SRC := $(RAYLIB_DIR)/src
RAYLIB_LIB := $(BUILD_DIR)/libraylib.a

# Auto-discover libraries and games
LIBS := $(shell find libs -maxdepth 1 -type d -not -path libs | sed 's|libs/||')
GAMES := $(shell find games -maxdepth 1 -type d -not -path games | sed 's|games/||')

# Build targets
LIB_TARGETS := $(LIBS:%=$(BUILD_DIR)/lib%.a)
GAME_TARGETS := $(GAMES:%=$(BUILD_DIR)/%)

# Include paths
INCLUDES := -I$(RAYLIB_DIR)/src
INCLUDES += $(shell find libs -name include -type d | sed 's/^/-I/')

# Library paths for linking
LDFLAGS := -L$(BUILD_DIR)
LDLIBS := $(RAYLIB_LIBS) -lraylib $(LIB_TARGETS:$(BUILD_DIR)/lib%.a=-l%)

# =============================================================================
# TARGETS
# =============================================================================

.PHONY: all clean libs games raylib help $(GAMES)
.DEFAULT_GOAL := all

# Enable parallel builds
ifeq ($(UNAME_S),Darwin)
  MAKEFLAGS += -j$(shell sysctl -n hw.ncpu)
else
  MAKEFLAGS += -j$(shell nproc)
endif

all: raylib libs games
	@echo "✓ Build completed ($(MODE) mode)"

raylib: $(RAYLIB_LIB)

libs: raylib $(LIB_TARGETS)

games: raylib libs $(GAME_TARGETS)

# Individual game targets
$(GAMES): %: $(BUILD_DIR)/%

# =============================================================================
# BUILD RULES
# =============================================================================

# Build raylib
$(RAYLIB_LIB): $(RAYLIB_SRC)/*.c
	@echo "Building raylib..."
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $(RAYLIB_SRC)/*.c
	@$(AR) rcs $@ *.o
	@rm -f *.o
	@echo "✓ Raylib built"

# Build shared libraries
$(BUILD_DIR)/lib%.a: libs/%
	@echo "Building library: $*"
	@mkdir -p $(BUILD_DIR)/obj/$*
	@find $< -name "*.c" -exec $(CC) $(CFLAGS) $(INCLUDES) -c {} -o $(BUILD_DIR)/obj/$*/{}.o \;
	@$(AR) rcs $@ $(BUILD_DIR)/obj/$*/*.o
	@echo "✓ Library $* built"

# Build games
$(BUILD_DIR)/%: games/%/src/*.c $(RAYLIB_LIB) $(LIB_TARGETS)
	@echo "Building game: $*"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) $< $(LDFLAGS) $(LDLIBS) -o $@
	@echo "✓ Game $* built"

# =============================================================================
# CONVENIENCE TARGETS
# =============================================================================

# Individual game builds
foss-flapper: $(BUILD_DIR)/foss_flapper

# Run games
run-%: $(BUILD_DIR)/%
	@echo "Running $*..."
	@cd $(dir $<) && ./$*

clean:
	@rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned"

help:
	@echo "C Gaming Monorepo Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all           Build everything"
	@echo "  clean         Clean build artifacts"
	@echo "  raylib        Build raylib only"
	@echo "  libs          Build shared libraries"
	@echo "  games         Build all games"
	@echo ""
	@echo "Individual builds:"
	@echo "  foss-flapper  Build FOSS Flapper game"
	@echo ""
	@echo "Running games:"
	@echo "  run-foss_flapper  Run FOSS Flapper"
	@echo ""
	@echo "Available games: $(GAMES)"
	@echo "Available libs:  $(LIBS)"
	@echo ""
	@echo "Build modes:"
	@echo "  make MODE=release  Optimized build (default)"
	@echo "  make MODE=debug    Debug build with symbols"