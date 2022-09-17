#version 431

#define FLT_EPSILON (.00000001f)

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
	inputBoid boids[];
}


Boid inputBoid_to_Boid(inputBoid input){
	Boid boid;
	boid.pos = vec3(input.pos[0], input.pos[1], input.pos[2]);
	boid.speed = vec3(input.speed[0], input.speed[1], input.speed[2]);
	boid.accel = vec3(input.accel[0], input.accel[1], input.accel[2]);
}

void Boid_to_buffer(Boid boid, uint id){
	boids[id].pos[0] = boid.pos.x;
	boids[id].pos[1] = boid.pos.y;
	boids[id].pos[2] = boid.pos.z;

	boids[id].speed[0] = boid.speed.x;
	boids[id].speed[1] = boid.speed.y;
	boids[id].speed[2] = boid.speed.z;

	boids[id].accel[0] = boid.accel.x;
	boids[id].accel[1] = boid.accel.y;
	boids[id].accel[2] = boid.accel.z;
}

void main(void){
	uint gid = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_GlobalInvocationID.x;

	if(gid < size){
		Boid curBoid = inputBoid_to_Boid(boids[gid]);

		int count_coherence = 0;
		vec3 tmp_coherence = vec3(0.0f);
		vec3 tmp_repulsion = vec3(0.0f);
		vec3 tmp_speedRegulationForce = vec3(0.0f);
		vec3 bary_neigh = vec3(0.0f);
		bool allSamePos = true;

		for(int i = 1; i<size; i++){
			Boid neighBoid = inputBoid_to_Boid(boids[(gid+i) % size]);
			float const dist = distance(curBoid, neighBoid);

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
			curBoid.accel += coherenceRate * tmp_coherence/(float) count_coherence;
			curBoid.accel += coherenceRate * (bary_neigh/(float) count_coherence - curBoid.pos);
		}
		else
			curBoid.accel += coherenceRate * (barycenter/(float) n - curBoid.pos);

		//repulsion
		if(allSamePos)
			tmp_repulsion += ballRand(1.0f);

		curBoid.accel = separationRate * tmp_repulsion;

		//speeedRegulationForce
		float const naturalDecay = .05f;
		if(all(lessThan(abs(curBoid.speed), vec3(FLT_EPSILON))))
			curBoid.accel = ballRand(10.0f);
		if(length(curBoid.speed) > )
			curBoid.accel *= (1 - naturalDecay);
		else
			curBoid.accel *= (1 + naturalDecay);

		//boxForce
		float const dist_to_box = length(curBoid.pos) - box_size;
		if(length(curBoid.pos) > box_size)
			curBoid.accel += wallRepulsionRate * (-normalize(curBoid.pos));


		Boid_to_buffer(curBoid, id);
	}
}
