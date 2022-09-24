#version 430

#define FLT_EPSILON (.00000001f)

layout(local_size_x = 32) in;
//only calculates sum of forces
uniform vec3 barycenter;

uniform float v0;
uniform float maxV;

uniform float separationRate;
uniform float wallRepulsionRate;  
uniform float perceptionDistance;
uniform float repulsionDistance;
uniform float box_size;
uniform float coherenceRate;

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

		int count_coherence = 0;
		vec3 tmp_coherence = vec3(0.0f);
		vec3 tmp_repulsion = vec3(0.0f);
		vec3 tmp_speedRegulationForce = vec3(0.0f);
		vec3 bary_neigh = vec3(0.0f);
		//bool allSamePos = true;

		for(int i = 1; i<size; i++){
			Boid neighBoid = mybuf.boids[(gid+i) % size];
			float dist = distance(mybuf.boids[gid].pos, neighBoid.pos);

			//coherence
			if(i != gid && dist < perceptionDistance){
				tmp_coherence += neighBoid.speed;
				bary_neigh += neighBoid.pos;
				count_coherence++;
			}

			//repulsion
			if(i != gid && dist < repulsionDistance){
				if(any(greaterThan(abs(mybuf.boids[gid].pos - neighBoid.pos), vec3(FLT_EPSILON)))){
					tmp_repulsion +=  normalize(mybuf.boids[gid].pos - neighBoid.pos)/(dist*dist);
					//allSamePos = false;
				}
				/*
				else
					if(i != gid) allSamePos = false;
				*/
			}
		}
		
		//coherence
		if(count_coherence > 0){
			mybuf.boids[gid].accel += coherenceRate * tmp_coherence/count_coherence;
			mybuf.boids[gid].accel += coherenceRate * (bary_neigh/count_coherence - mybuf.boids[gid].pos);
		}
		else
			mybuf.boids[gid].accel += coherenceRate * (barycenter/size - mybuf.boids[gid].pos);
		

		//repulsion
		//if(allSamePos)
			//tmp_repulsion += ballRand(1.0f);


		mybuf.boids[gid].accel += separationRate * tmp_repulsion;

		//speeedRegulationForce
		float naturalDecay = .05f;
		if(all(lessThan(abs(mybuf.boids[gid].speed), vec3(FLT_EPSILON))))
			//mybuf.boids[gid].accel = ballRand(10.0f);
			mybuf.boids[gid].accel = vec3(10.0f);
		if(length(mybuf.boids[gid].speed) > v0)
			mybuf.boids[gid].accel *= (1 - naturalDecay);
		else
			mybuf.boids[gid].accel *= (1 + naturalDecay);

		//boxForce
		float dist_to_box = length(mybuf.boids[gid].pos) - box_size;
		if(length(mybuf.boids[gid].pos) > box_size)
			mybuf.boids[gid].accel += wallRepulsionRate * (-normalize(mybuf.boids[gid].pos));

		float max_accel = 100.0f;

		if(length(mybuf.boids[gid].accel) > max_accel){
			mybuf.boids[gid].accel = max_accel * normalize(mybuf.boids[gid].accel);
		}
		//Boid_to_buffer(mybuf.boids[gid], gid);
	}
}
