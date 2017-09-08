# Large Game Project
[![Travis CI Build Status](https://travis-ci.org/Chainsawkitten/LargeGameProjectEngine.svg?branch=master)](https://travis-ci.org/Chainsawkitten/LargeGameProjectEngine)
[![AppVeyor CI Build Status](https://ci.appveyor.com/api/projects/status/bmewjuam0j9h7qq9?svg=true)](https://ci.appveyor.com/project/Chainsawkitten/largegameprojectengine)

C++/OpenGL 3D Engine. See the [wiki](https://github.com/Chainsawkitten/LargeGameProjectEngine/wiki) for usage.

## Building
1. Clone the GitHub repository.
2. Run `git submodule update --init`
3. Use CMake to generate make/project files for Hymn to Beauty.
4. Build Hymn to Beauty.

## Setting up game project
On Windows:
1. Open command line.
2. Go to `%appdata%/Hymn to Beauty/Hymns`
3. Type `mklink /J Game <Path to repository folder>/Game`

## Measuring VRAM
Measuring VRAM requires building with Visual Studio and having the Windows SDK installed. Enable the `MeasureVram` option in CMake.

## License
Released under the [MIT license](LICENSE).

Hymn to Beauty uses several third party libraries with their own licenses:

| Library  | License |
| --- | --- |
| [AngelScript](https://github.com/IngwiePhoenix/AngelScript) | zlib |
| [assimp](https://github.com/assimp/assimp) | Modified BSD |
| [Headerize](https://github.com/Chainsawkitten/Headerize) | Public domain |
| [GLEW](https://github.com/Perlmint/glew-cmake) | Modified BSD / MIT |
| [GLFW](https://github.com/glfw/glfw) | zlib / libpng |
| [GLM](https://github.com/g-truc/glm) | Modified MIT / MIT |
| [dear imgui](https://github.com/ocornut/imgui) | MIT |
| [JsonCpp](https://github.com/open-source-parsers/jsoncpp) | MIT / Public domain |
| [OpenAL Soft](https://github.com/kcat/openal-soft) | LGPL |
| [SimpleIni](https://github.com/brofield/simpleini) | MIT |
| [stb](https://github.com/nothings/stb) | Public domain |
