#version 430

#define FLT_EPSILON (.00000001f)

layout(local_size_x = 2048) in;
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



layout(std430, binding = 0) coherent buffer boidBuffer{
	inputBoid boids[];
} mybuf;


Boid inputBoid_to_Boid(inputBoid inBoid){
	Boid boid;

	boid.pos = vec3(inBoid.pos[0], inBoid.pos[1], inBoid.pos[2]);
	boid.speed = vec3(inBoid.speed[0], inBoid.speed[1], inBoid.speed[2]);
	boid.accel = vec3(0.0f);
	
	return boid;
}

void Boid_to_buffer(Boid boid, uint id){

	mybuf.boids[id].accel[0] = boid.accel.x;
	mybuf.boids[id].accel[1] = boid.accel.y;
	mybuf.boids[id].accel[2] = boid.accel.z;
}

void main(void){
	uint gid = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_GlobalInvocationID.x;

	//uint gid = gl_GlobalInvocationID.x;

	if(gid < size){
		Boid curBoid = inputBoid_to_Boid(mybuf.boids[gid]);

		int count_coherence = 0;
		vec3 tmp_coherence = vec3(0.0f);
		vec3 tmp_repulsion = vec3(0.0f);
		vec3 tmp_speedRegulationForce = vec3(0.0f);
		vec3 bary_neigh = vec3(0.0f);
		bool allSamePos = true;

		for(int i = 1; i<size; i++){
			Boid neighBoid = inputBoid_to_Boid(mybuf.boids[(gid+i) % size]);
			float dist = distance(curBoid.pos, neighBoid.pos);

			//coherence
			if(i != gid && dist < perceptionDistance){
				tmp_coherence += neighBoid.speed;
				bary_neigh += neighBoid.pos;
				count_coherence++;
			}

			//repulsion
			if(i != gid && dist < repulsionDistance){
				if(any(greaterThan(abs(curBoid.pos - neighBoid.pos), vec3(FLT_EPSILON)))){
					tmp_repulsion +=  normalize(curBoid.pos - neighBoid.pos)/(dist*dist);
					allSamePos = false;
				}
				else
					if(i != gid) allSamePos = false;
			}
		}
		
		//coherence
		if(count_coherence > 0){
			curBoid.accel += coherenceRate * tmp_coherence/count_coherence;
			curBoid.accel += coherenceRate * (bary_neigh/count_coherence - curBoid.pos);
		}
		else
			curBoid.accel += coherenceRate * (barycenter/size - curBoid.pos);
		

		//repulsion
		//if(allSamePos)
			//tmp_repulsion += ballRand(1.0f);


		curBoid.accel += separationRate * tmp_repulsion;

		//speeedRegulationForce
		float naturalDecay = .05f;
		if(all(lessThan(abs(curBoid.speed), vec3(FLT_EPSILON))))
			//curBoid.accel = ballRand(10.0f);
			curBoid.accel = vec3(10.0f);
		if(length(curBoid.speed) > v0)
			curBoid.accel *= (1 - naturalDecay);
		else
			curBoid.accel *= (1 + naturalDecay);

		//boxForce
		float dist_to_box = length(curBoid.pos) - box_size;
		if(length(curBoid.pos) > box_size)
			curBoid.accel += wallRepulsionRate * (-normalize(curBoid.pos));

		Boid_to_buffer(curBoid, gid);
	}
}
