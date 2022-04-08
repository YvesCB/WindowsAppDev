# About this Repo
Trying to build Apps in C/C++

Currently working on building a fundamental understanding of Computer graphics by implementing some basic rendering methods from scratch. 

The platform layer of this project is inspired by the [HandmadeHero project](https://handmadehero.org/). Some of the math used is based on the [Scratchpixel 2.0 blog](https://www.scratchapixel.com/index.php?redirect).

## Note about languages Used
Most of the code will be in .cpp files. However, the code contained in those files will be almost exclusively C code. I don't intend to use many C++ features at all and there won't be any OOP in this project.

If I ever wanted to change that, I would make a new project for that purpose.

## AppDevTest
This was the initial test project. The fundamental parts of which are just based on [Microsoft's own tutorial](https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program). On each keyboard input, this program will re-draw the pixel buffer and display a simple parabola on the screen.

This software uses a very naive implementation of Newton's method to minimize the distance function between each pixel and the curve to achieve a curve that is multiple pixels thick. The same algorithm is also used for optimizing the performance. We first run the algorithm on a overlayed grid of larger cells, each consisting of a few hundred pixels (this value is configurable), to determine whether we actually need to do the computation for each pixel in that cell. This saves us a ton of computation and actually makes the code run fairly fast. A lot more could be done but I'm decently happy with it. 

It also implements a very simple anti-aliasing method. Using some vector geometry, the code determines an approximation of how much of the pixel is coverd by the curve. Either 0%, 25%, 75% or 100%. Then it will use that to mix in the backgrond color to give a smoothing effect. This too works decently well and comes at little performance cost.

## RenderEngine
In this project I will attempt to implement some rasterization and vector geometry methods of displaying 3D graphics. So far it is only the bare bones. Currently I'm trying to render some triangles on the screen by casting rays through the pixel raster and intersecting them with the geometry I define in memory.

## Building the code
If you want to build this code, make sure that you have the C/C++ build tools installed via Visual Studio on Windows. The build script relies on `vcvars64.bat` being included in `%PATH%`. You can find this batch script in your Visual Studio directory. Without it, the linker won't be able to find the required libraries.
