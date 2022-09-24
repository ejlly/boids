#version 430

#define FLT_EPSILON (.00000001f)

layout(local_size_x = 32) in;

uniform float v0;
uniform float maxV;
uniform float time;

uniform uint size;

struct Boid{
	vec3 pos;
	vec3 speed;
	vec3 accel;
};

layout(std430, binding = 0) coherent buffer boidBuffer{
	Boid boids[];
} mybuf;


void main(void){
	//uint gid = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
	uint gid = gl_GlobalInvocationID.x;

	if(gid < size){
		//new speed
		mybuf.boids[gid].speed += time*mybuf.boids[gid].accel;
		float norm_speed = length(mybuf.boids[gid].speed);
		vec3 dir = normalize(mybuf.boids[gid].speed);


		if(norm_speed > maxV)
			mybuf.boids[gid].speed = dir*maxV;

		mybuf.boids[gid].speed = dir*(norm_speed * .9f + v0*.1f);


		//new position
		mybuf.boids[gid].pos += time*mybuf.boids[gid].speed;


		mybuf.boids[gid].accel = vec3(0.0f);
	}
}
