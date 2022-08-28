#ifndef BOIDS_HPP
#define BOIDS_HPP

class Boid{
	float pos[3];
	float dir[3];
	float v[3];
	float a[3];

	Boid();

	float distance(Boid &b);
};

#endif
