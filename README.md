# MandelbrotGPU
A very simple Mandelbrot explorer using OpenGL, with shaders in C. Computes the Mandelbrot set on the GPU. Uses the simplest design possible. Written for Windows, but it would be very easy to port.

glfMB.h contains the GLSL for the fragment shader, which computes the Mandelbrot set and sets each pixel on the screen.
Main.c includes the setup to run the fragment shader and gets user input. Includes glfMB.h later on in the file.

A (highly) scaled down capture:
![Scaled down image](capture.png)
