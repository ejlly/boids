#include "boids.hpp"
#include <iostream>

float anglef(glm::vec3 const &a, glm::vec3 const &b){
	//a and b are already normalized
	return glm::acos(glm::dot(a, b));
}

Boid::Boid(){
	pos = glm::vec3(0.0f);
	speed = glm::vec3(0.0f);
	accel = glm::vec3(0.0f);
}

Boid::Boid(Boid &tmpboid){
	pos = tmpboid.pos;
	//pos = glm::vec3(tmpboid.pos[0], tmpboid.pos[1], tmpboid.pos[2]);
	//std::cout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
	speed = tmpboid.speed;
	//speed = glm::vec3(tmpboid.speed[0], tmpboid.speed[1], tmpboid.speed[2]);
	//std::cout << speed[0] << " " << speed[1] << " " << speed[2] << std::endl;
	accel = tmpboid.accel;
	//accel = glm::vec3(tmpboid.accel[0], tmpboid.accel[1], tmpboid.accel[2]);
	//std::cout << accel[0] << " " << accel[1] << " " << accel[2] << std::endl;
}

Boid::Boid(GpuBoid tmpboid){
	pos = tmpboid.pos;
	//pos = glm::vec3(tmpboid.pos[0], tmpboid.pos[1], tmpboid.pos[2]);
	//std::cout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
	speed = tmpboid.speed;
	//speed = glm::vec3(tmpboid.speed[0], tmpboid.speed[1], tmpboid.speed[2]);
	//std::cout << speed[0] << " " << speed[1] << " " << speed[2] << std::endl;
	accel = tmpboid.accel;
	//accel = glm::vec3(tmpboid.accel[0], tmpboid.accel[1], tmpboid.accel[2]);
	//std::cout << accel[0] << " " << accel[1] << " " << accel[2] << std::endl;
}

float const Boid::distance(Boid const &b){
	return glm::distance(b.pos, pos);
}

glm::vec3 const Boid::dir(){
	return glm::normalize(speed);
}

void Boid::update(float time){
	
	//new speed
	speed += time*accel;
	glm::vec3 const dir = this->dir();

	if(glm::length(speed) > maxV)
		speed = dir*maxV;

	speed = dir*(glm::length(speed) * .9f + v0*.1f);


	//new position
	pos += time*speed;


	accel = glm::vec3(0.0f);
}

void Boid::get_model(glm::mat4 &model){
	model = glm::translate(model, pos);
	glm::vec3 dir = glm::normalize(speed);
	if(glm::any(glm::greaterThan(glm::abs(dir - asset_orientation), glm::vec3(FLT_EPSILON))))
		model = glm::rotate(model, -glm::acos(glm::dot(dir, asset_orientation)), glm::cross(dir, asset_orientation));
}

void Flock::coherenceForce(){
	int const n = m_size;
	
	glm::vec3 barycenter(0.0f);

	for(int i(0); i<n; i++){
		barycenter += boids[i].pos;
		int count(0);
		glm::vec3 tmp(0.0f), bary_neigh(0.0f);
		for(int j(0); j<n; j++){
			float const dist(boids[i].distance(boids[j]));
			if(i != j && dist < perceptionDistance){
				tmp +=  boids[j].speed;
				bary_neigh += boids[j].pos;
				count++;
			}
		}
		
		if(count > 0){
			boids[i].accel += coherenceRate * tmp/(float) count;
			boids[i].accel += coherenceRate * (bary_neigh/(float) count - boids[i].pos);
		}
		else
			boids[i].accel += coherenceRate * (barycenter/(float) n - boids[i].pos);
	}
}

void Flock::repulsionForce(){
	int const n = m_size;
	
	for(int i(0); i<n; i++){
		glm::vec3 const dir = boids[i].dir();
		glm::vec3 tmp(0.0f);
		bool allSamePos(true);
		for(int j(0); j<n; j++){
			float const dist(boids[i].distance(boids[j]));
			if(i != j && dist < repulsionDistance){
				if(glm::any(glm::greaterThan(glm::abs(boids[i].pos - boids[j].pos), glm::vec3(FLT_EPSILON)))){
					tmp +=  glm::normalize(boids[i].pos - boids[j].pos)/(dist*dist);
					allSamePos = false;
				}
			}
			else
				if(i != j) allSamePos = false;
		}
		if(allSamePos)
			tmp += glm::ballRand(1.0f);
		boids[i].accel += tmp * separationRate;
	}
}

void Flock::boxForce(){
	for(int i(0); i<m_size; i++){
		float const dist = (glm::length(boids[i].pos)-box_size);
		if(glm::length(boids[i].pos) > box_size){
			boids[i].accel += wallRepulsionRate * (-glm::normalize(boids[i].pos));
		}
	}
}

void Flock::speedRegulationForce(){
	float const naturalDecay = .05f;

	for(int i(0); i<m_size; i++){
		glm::vec3 tmp(0.0f);
		if(glm::all(glm::lessThan(glm::abs(boids[i].speed), glm::vec3(FLT_EPSILON))))
			boids[i].accel = glm::ballRand(10.0f);
		if(glm::length(boids[i].speed) > v0)
			boids[i].accel *= (1 - naturalDecay);
		else
			boids[i].accel *= (1 + naturalDecay);
	}
}

bool Flock::add_boid(){
	if(m_size >= MAX_BOIDS) return false;
	boids[m_size++] = Boid();
	return true;
}

void Flock::init_boids(unsigned int nbBoids){
	if(nbBoids > MAX_BOIDS) nbBoids =  MAX_BOIDS;
	for(int i(0); i<nbBoids; i++){
		Boid tmp;
		tmp.pos = glm::ballRand(10.0f);
		tmp.speed = glm::ballRand(v0);
		boids[m_size++] = tmp;
	}
}

unsigned int Flock::size(){
	return m_size;
}

void Flock::update(){

	coherenceForce();
	repulsionForce();
	speedRegulationForce();
	boxForce();

	for(int i(0); i<m_size; i++){
		boids[i].update(.15);
	}
}

Boid& Flock::operator[](int i){
	return boids[i];
}
