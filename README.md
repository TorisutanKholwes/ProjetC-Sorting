# SDL3 Template Application

A comprehensive base template for SDL3 applications written in C, featuring a modular UI framework with buttons, text rendering, input boxes, and frame management.

## 🎯 Features

- **Modern SDL3 Integration**: Built with SDL3 and its extensions (SDL3_image, SDL3_mixer, SDL3_ttf)
- **UI Component Library**: Pre-built components including buttons, text elements, input boxes, and frames
- **Resource Management**: Centralized resource manager for fonts, images, and audio
- **Frame-based Navigation**: Stack-based frame system for managing different screens/views
- **Theming Support**: Built-in theming system for consistent UI styling
- **Input Handling**: Comprehensive keyboard and mouse input management
- **Logging System**: Flexible logging with different severity levels
- **Utility Functions**: String builders, lists, maps, and geometry helpers

## 📋 Prerequisites

Before building this project, ensure you have the following installed:

- **CMake** (version 3.22 or higher)
- **C Compiler** with C23 support (GCC, Clang, or MSVC)
- **Git** (for cloning SDL3 repositories if needed)

### Platform-specific Requirements

**Linux/macOS:**
- Build essentials (`build-essential` on Ubuntu/Debian, Xcode Command Line Tools on macOS)
- Development libraries for X11, OpenGL, ALSA, PulseAudio (Linux only)

**Windows:**
- Visual Studio 2019 or later (with C++ Desktop Development workload)
- Or MinGW-w64 with GCC

## 🚀 Installation

### Clone the Repository

```bash
git clone https://github.com/TorisutanKholwes/SDL_Base.git
cd SDL_Base
```

## 🔨 Building the Project

The project uses CMake for building and will automatically attempt to install SDL3 and its extensions if they are not found on your system.

### Standard Build Process

#### On Linux/macOS:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### On Windows (PowerShell):

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

#### On Windows (Command Prompt):

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Running the Application

After successful build:

#### On Linux/macOS:
```bash
./SDLBase
```

#### On Windows:
```cmd
.\Release\SDLBase.exe
```
Or from the build directory:
```cmd
.\SDLBase.exe
```

## 🔧 Manual SDL3 Installation

If the automatic SDL3 installation during CMake configuration fails, you can manually install SDL3 using the provided scripts:

### On Linux/macOS (Bash):

```bash
# Install to default location (./libs)
bash install_libs.sh

# Or specify a custom installation directory
bash install_libs.sh /path/to/install/dir
```

After manual installation, configure CMake with the installation path:

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/install/dir ..
cmake --build .
```

### On Windows (PowerShell):

```powershell
# Install to default location (./libs)
powershell -ExecutionPolicy Bypass -File .\install_sdl3.ps1

# Or specify a custom installation directory
powershell -ExecutionPolicy Bypass -File .\install_sdl3.ps1 -InstallDir "C:\path\to\install\dir"
```

After manual installation, configure CMake with the installation path:

```powershell
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="C:\path\to\install\dir" ..
cmake --build . --config Release
```

**Note:** The installation scripts will clone, build, and install the following SDL3 libraries:
- SDL3 (core library)
- SDL3_image (image loading support)
- SDL3_mixer (audio mixing)
- SDL3_ttf (TrueType font rendering)

## ⚙️ Configuration

The application can be configured by modifying the `include/Settings.h` file:

```c
#define APP_NAME "SDL3 Template"           // Application name
#define WINDOW_WIDTH 800                   // Initial window width
#define WINDOW_HEIGHT 600                  // Initial window height
#define WINDOW_TITLE "SDL3 Template Application"  // Window title
#define FULLSCREEN 0                       // 0 for windowed, 1 for fullscreen
#define FRAME_RATE 60                      // Target frame rate
#define PRODUCTION 0                       // 0 for development, 1 for production (disable debug logs)
```

After modifying settings, rebuild the project:

```bash
cmake --build . --target SDLBase
```

## 📁 Project Structure

```
SDL_BasePrivate/
├── assets/              # Asset files (fonts, images, audio)
│   └── fonts/          # Font files
├── include/            # Header files
│   ├── app.h          # Main application structure
│   ├── Settings.h     # Configuration settings
│   ├── button.h       # Button UI component
│   ├── frame.h        # Frame management
│   ├── input.h        # Input handling
│   ├── logger.h       # Logger system
│   ├── resource_manager.h  # Resource management
│   └── ...            # Other component headers
├── src/               # Source files
│   ├── main.c        # Application entry point
│   ├── app.c         # Application logic
│   ├── button.c      # Button implementation
│   ├── frame.c       # Frame implementation
│   └── ...           # Other component implementations
├── CMakeLists.txt    # CMake build configuration
├── install_sdl3.sh   # SDL3 installation script (Bash)
├── install_sdl3.ps1  # SDL3 installation script (PowerShell)
└── README.md         # This file
```

## 🎨 Usage Example

The project includes example frames demonstrating the UI framework:

1. **Main Frame**: Entry point with buttons and UI elements
2. **Second Frame**: Additional screen showing frame navigation

You can create new frames by:
1. Creating a new header file in `include/` (e.g., `my_frame.h`)
2. Creating the implementation in `src/` (e.g., `my_frame.c`)
3. Implementing the frame creation, event handling, update, and render functions
4. Adding the frame to the application stack in `main.c`

## 🐛 Troubleshooting

### Build Issues

**Problem**: CMake can't find SDL3 libraries
- **Solution**: Run the manual installation scripts as described above, then reconfigure CMake with `-DCMAKE_PREFIX_PATH`

**Problem**: Compiler errors about C23 features
- **Solution**: Ensure your compiler supports C23, or modify `CMakeLists.txt` to use C11/C17

**Problem**: Linking errors on Linux
- **Solution**: Install development libraries: `sudo apt-get install libx11-dev libxext-dev libgl1-mesa-dev libasound2-dev libpulse-dev`

### Runtime Issues

**Problem**: "Unable to create window" error
- **Solution**: Ensure your system has proper graphics drivers and X11/Wayland support (Linux) or display drivers (Windows)

**Problem**: "Unable to open audio device" error
- **Solution**: Check audio system is properly configured and not used by other applications

## 📝 License

Copyright (c) 2025 Torisutan. All rights reserved.

## 👤 Author

**Torisutan Kholwes**

## 🤝 Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## 📚 Resources

- [SDL3 Official Documentation](https://wiki.libsdl.org/SDL3/FrontPage)
- [SDL3 GitHub Repository](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)