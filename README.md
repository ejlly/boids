# Boids simulation

Realtime boids simulation of 3D boids using OpenGL

[!](https://user-images.githubusercontent.com/96349904/192846495-7aab3824-ebcb-49cd-b49f-850976ecbd11.mp4)

# Principle

We simulate boids in a 3D environment. This is based on the model made by Craig W. Reynolds (https://en.wikipedia.org/wiki/Boids).

It uses a naive implementation where each boid checks all the other boids to determine where it goes next.

Shortly put, each boid is subjected to :
- a coohesion force, which makes it match the velocity and direction of its neighbours

- a repulsion force, which makes it evade collisions

- a box-keeping force, which makes it stay near the origin

# Controls

- Use mouse and arrow keys to navigate

# Installation

### Only tested on Linux

First move to the folder :
`cd boids`


Use cmake to generate makefile :
`cmake .`

Then use make to compile :
`make`

You can then run the executable with :
`./boidsSim`
