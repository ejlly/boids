#version 431

//only calculates sum of forces
uniform vec3 barycenter;

uniform float maxV;

uniform float separationRate;
uniform float wallRepulsionRate;  
uniform float perceptionDistance;
uniform float repulsionDistance;
uniform float box_size;
uniform float coherenceRate;

layout(local_size = 1024) in;

struct inputBoid{
	float pos[3];
	float speed[3];
	float accel[3];
};

struct Boid{
	vec3 pos;
	vec3 speed;
	vec3 accel;
};


layout(std430, binding = 0) buffer boidBuffer{
	Boid boids[];
}

layout(location = 1) uint size;

Boid inputBoid_to_Boid(inputBoid input){
	Boid boid;
	boid.pos = vec3(input.pos[0], input.pos[1], input.pos[2]);
	boid.speed = vec3(input.speed[0], input.speed[1], input.speed[2]);
	boid.accel = vec3(input.accel[0], input.accel[1], input.accel[2]);
}


inputBoid Boid_to_inputBoid(Boid boid){
	inputBoid input;
	input.pos[0] = boids.pos.x;
	input.pos[1] = boids.pos.y;
	input.pos[2] = boids.pos.z;

	input.speed[0] = boids.speed.x;
	input.speed[1] = boids.speed.y;
	input.speed[2] = boids.speed.z;

	input.accel[0] = boids.accel.x;
	input.accel[1] = boids.accel.y;
	input.accel[2] = boids.accel.z;
}

void main(void){
	
}
