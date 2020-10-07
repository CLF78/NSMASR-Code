# Newer Super Mario All-Stars Revived

### Licensing
This is a fork of the Newer Super Mario Bros. Wii source code, which is released under the MIT license. See the `LICENSE` file in this repository for full details.

Copyright(c) 2010-2013 Treeki, Tempus, megazig

### Kamek
Kamek is a simple build system for calling the tools to compile/assemble each source file and putting together an output file that can be injected into the game.
* Kamek is configured through a game-specific project file, which pulls in various modules that can contain both C++ and assembly source files.
* Code injection works through different types of hooks: they can patch static code/data in RAM, overwrite pointers with a reference to a symbol exported from Kamek code or inject branch instructions.
* Memory addresses in the linker script are automatically converted to match all supported game versions - Kamek uses the original version of the PAL/European NSMBW release as a reference.
* The compiled output is converted to a specific format expected by the Newer loader.

A tutorial on how to setup this version will be provided soon. Requirements are below.

### Requirements
* [CodeWarrior for MPC55xx/MPC56xx v2.10 Special Edition](https://www.nxp.com/design/software/development-software/codewarrior-development-tools/codewarrior-legacy/codewarrior-development-suite-special:CW-SUITE-SPECIAL?tab=Design_Tools_Tab) (press `MORE` at the bottom of the page to find it)
* [devkitPPC r33](https://wii.leseratte10.de/devkitPro/devkitPPC/r33%20(2018)/) (or older)
* Python 3.x
* Python Libraries: PyYAML, pyelftools

### Other Assets
The rest of the folders contain the game's maps and map tilesets, a layout in .rlyt format, a randtiles script and an animtiles file.

### Credits
* Treeki, Tempus and megazig for the original Newer Super Mario Bros. Wii repo
* RoadRunnerWMC, for providing a Python 3 version of Kamek
* Skogcatt, for the CodeWarrior license patch
