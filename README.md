# C Gaming Monorepo with Raylib

A complete, scalable, and self-contained C gaming monorepo using the raylib library. Professional starting point for developing multiple 2D pixel art games in C, featuring a complete Flappy Bird clone called "FOSS Flapper".

## Quick Start

### Prerequisites

```bash
# macOS
xcode-select --install

# Ubuntu/Debian  
sudo apt-get install build-essential clang make git

# Arch Linux
sudo pacman -S base-devel clang make git
```

### Build and Run

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/dunamismax/c-raylib
cd c-raylib

# Build everything
make

# Run the demo game
./build/foss_flapper
```

### Individual Targets

```bash
make clean              # Clean build artifacts
make raylib             # Build raylib only
make libs               # Build shared libraries
make games              # Build all games
make foss-flapper       # Build FOSS Flapper specifically
make run-foss_flapper   # Build and run FOSS Flapper
```

## Project Structure

```
c-raylib/
├── assets/                    # Game assets
│   └── foss_flapper/
│       ├── audio/            # Sound effects and music
│       └── textures/         # Sprites and images
├── games/                    # Individual games
│   └── foss_flapper/
│       ├── src/             # Game source code
│       └── assets/          # Symlink to ../../assets/foss_flapper
├── libs/                     # Shared libraries
│   └── corelib/
│       ├── src/             # Library source code
│       └── include/         # Library headers
├── vendor/                   # Third-party dependencies
│   └── raylib/              # Raylib submodule
├── Makefile                 # Build system
└── README.md               # This file
```

### Directory Purpose

- **`games/`**: Contains individual game projects. Each game has its own directory with source code and assets.
- **`libs/`**: Shared libraries used across multiple games. Currently includes `corelib` with animation system.
- **`vendor/`**: Third-party dependencies managed as git submodules. Contains the official raylib library.
- **`assets/`**: Centralized asset storage that can be shared between games via symlinks.

## FOSS Flapper Game

A complete Flappy Bird clone featuring:

- **Game States**: Ready screen, playing mode, and game over screen
- **Physics**: Gravity-based bird movement with jump mechanics
- **Procedural Generation**: Infinite scrolling pipes with randomized gaps
- **Collision Detection**: Circle-to-rectangle collision system
- **Scoring**: Score tracking with high score persistence
- **Responsive Controls**: Mouse click or spacebar to play

### Game Controls

- **Mouse Click** or **Spacebar**: Make the bird flap
- **Any Key**: Restart after game over

## Core Library (corelib)

The shared `corelib` provides common game functionality:

### Animation System

```c
// Create an animation from spritesheet frames
Animation CreateAnimation(Texture2D spritesheet, Rectangle* frames, 
                         int frameCount, float frameDuration, bool loop);

// Update animation timing
void UpdateAnimation(Animation* anim, float deltaTime);

// Draw current animation frame
void DrawAnimation(Animation anim, Vector2 position, float rotation);
```

## Build System Features

The Makefile provides a comprehensive build system with:

- **Auto-Discovery**: Automatically finds and builds all games and libraries
- **Cross-Platform**: Supports macOS, Linux with appropriate compiler flags
- **Optimization**: ARM64-specific optimizations for Apple Silicon
- **Parallel Builds**: Uses all available CPU cores
- **Raylib Integration**: Builds raylib from source and links correctly
- **Clean Dependencies**: Proper dependency tracking for incremental builds

### Build Modes

```bash
make MODE=release    # Optimized build (default)
make MODE=debug      # Debug build with symbols
```

### Platform Support

- **macOS**: Native Apple Silicon optimization with required frameworks
- **Linux**: OpenGL and X11 integration
- **Cross-compilation ready** for other platforms

## Development Workflow

1. **Add New Game**: Create directory in `games/` with `src/` subdirectory
2. **Add Assets**: Place in `assets/game_name/` and create symlink in game directory
3. **Use Shared Code**: Include `corelib.h` and link against the library
4. **Build**: Run `make` - new games are auto-discovered
5. **Test**: Use `make run-game_name` to build and run

## Technical Details

- **C Standard**: C11 with modern compiler features
- **Graphics**: raylib for cross-platform graphics, audio, and input
- **Memory Management**: Safe memory practices with proper cleanup
- **Performance**: Optimized for 60 FPS gaming with efficient rendering
- **Modularity**: Clean separation between games and shared libraries

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add your game or library improvements  
4. Ensure `make` builds without errors
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Built for Modern C Game Development**  
*A professional foundation for creating multiple games with shared libraries and optimized build system*