# Near-Real-Time-Ray-Tracer

A multithreaded CPU-only ray tracer made with SDL and written in C++.
The project was created during my final year in the DAE game development course as a thesis.

A short demo video:

[![Video](https://img.youtube.com/vi/PsosLZ1BMKc/0.jpg)](https://www.youtube.com/watch?v=PsosLZ1BMKc)

## Getting Started/ Running the project

After getting a copy of the project, you will find an executable build of the project in the [Release folder](github.com/DePotter-Mathias/Near-RealTime-Ray-Tracer/tree/master/Near Real-Time Ray Tracer/Release/) .
simply run 'Raytracer.exe'.

## Expected performance

The program should run at about ~18-25 fps or ~3-5 fps depending on if Depth of Field is enabled.

## Effects Implemented

* (Diffuse) Reflection
* Opacity
* Refraction
* Depth of field
* Dynamic focal distance
* Soft shadows
* Multiple colored point lights
* Ambient light
* Blinn-Phong shading
* Normal maps and Textures
* Async multithreaded recursive function
* Monte Carlo sampling (for depth of field and soft shadows).

## Possible future changes

* GPU hardware acceleration, as it is now running purly on the cpu.

## Stuff used

* [SDL](https://www.libsdl.org/) - The development library
* [SDL_TTF](https://www.libsdl.org/projects/SDL_ttf/) - A complementary SDL library for displaying TrueType fonts
* [SDL_image](https://www.libsdl.org/projects/SDL_image/) - A complementary SDL library for reading and writing images
* [Visual Studio](https://visualstudio.microsoft.com/) - IDE of choice

## Acknowledgments and thanks
* Dieter Tack
* Arno De Keukelaere
* Sam Lantinga
* Mattias Engdegård
* SDL and SDL Community
* and anyone whose snippets were used
