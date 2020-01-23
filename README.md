# MandelbrotGPU
A very simple Mandelbrot explorer using OpenGL shaders in C. Uses the simplest design possible. Written for Windows, but it would be very easy to port.

glfMB.h contains the GLSL for the fragment shader which computes the mandelbrot set and sets each pixel on the screen.
Main.c includes the setup to run the fragment shader and gets user input. Includes glfMB.h later on in the file.
