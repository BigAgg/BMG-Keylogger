# BMG-Keylogger
BMG-Keylogger aims to simplify and encrypt the use of NFC-Chips with multiple users. The program is free to use under the [MIT License](https://mit-license.org/).

## Core
The **Core** is built using [Raylib](https://www.raylib.com/) for rendering, [ImGuI](https://github.com/ocornut/imgui) for menu handling and [rlImGuI](https://github.com/raylib-extras/rlImGui) to wrap ImGuI into Raylib.

## User handling
Saved users are handled using 3 components:
- First Name 
- Last Name 
- Key 
When they are saved, the readable strings are encrypted so they are not stored in plain text on the drive.

## Cloning the repo:
```sh
git clone --recurse-submodules https://github.com/BigAgg/BMG-Keylogger.git
```

## Resource Management:
If you want to add additional Resources to the Project like Button png's etc. you should create a new folder in the cloned directory called **res/** and store your Resources here. They will be copied automatically to the binary location after building. 

**Keep in Mind, that you have to create it BEFORE Preparing the Project (Step down below).**

## Building the Keylogger
### Windows:
***Requirements***:
- [CMake](https://cmake.org/) Version 3.8 or higher
- Any c++ Compiler that supports c++20 or higher like gcc

### Preparing the Project:
cd into the cloned repository, create a folder named *build* and cd into the *build* folder
```sh
mkdir build
cd build
cmake ..
```

### Building debug:
cd into the build directory *Preparing the Project* and build it in debug:
```sh
cmake --build .
```
After building the Project in **debug mode** you can find the **Executable** under "build/Debug/BMG-Keylogger.exe"

### Building release:
cd into the build directory *Preparing the Project* and build it in release:
```sh
cmake --build . --config Release
```
After building the Project in **release mode** you can find the **Executable** under "build/Release/BMG-Keylogger.exe"

### Linux:
Linux is currently not supported but it is planned to be in the future!

## Plans for future Updates
- Select directory and filename to store userdata
- More customizable users for example adding own hints and keys dynamically
- Different NFC Reader support 

## Versioning
- **1.0** First Release
    - Added Feature to create users
    - Added Feature to edit existing users
    - Added Feature to save and restore users
    - Added Feature to read and encrypt an NFC Key using the ACR122U NFC Reader

## Resources
[Raylib](https://www.raylib.com/)

[rlImGui](https://github.com/raylib-extras/rlImGui)

[ImGuI](https://github.com/ocornut/imgui)

## License
[MIT](https://mit-license.org/)
