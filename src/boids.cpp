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
	steerTwds = glm::vec3(0.0f);
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
	steerTwds = glm::vec3(0.0f);
}

void Boid::get_model(glm::mat4 &model){
	model = glm::translate(model, pos);
	glm::vec3 dir = glm::normalize(speed);
	if(glm::any(glm::greaterThan(glm::abs(dir - asset_orientation), glm::vec3(FLT_EPSILON))))
		model = glm::rotate(model, -glm::acos(glm::dot(dir, asset_orientation)), glm::cross(dir, asset_orientation));
}

void Flock::coherenceForce(){
	int const n = boids.size();
	
	auto it(boids.begin());

	glm::vec3 barycenter(0.0f);

	for(int i(0); i<n; i++, it++){
		barycenter += it->pos;
		auto it_neigh(boids.begin());
		int count(0);
		glm::vec3 tmp(0.0f), bary_neigh(0.0f);
		for(int j(0); j<n; j++, it_neigh++){
			float const dist(it->distance(*it_neigh));
			if(i != j && dist < perceptionDistance){
				tmp +=  it_neigh->speed;
				bary_neigh += it_neigh->pos;
				count++;
			}
		}
		
		if(count > 0){
			it->accel += coherenceRate * tmp/(float) count;
			it->accel += coherenceRate * (bary_neigh/(float) count - it->pos);
		}
		else
			it->accel += coherenceRate * (barycenter/(float) n - it->pos);
	}
}

void Flock::repulsionForce(){
	int const n = boids.size();
	
	auto it(boids.begin());

	for(int i(0); i<n; i++, it++){
		auto it_neigh(boids.begin());
		glm::vec3 const dir = it->dir();
		glm::vec3 tmp(0.0f);
		bool allSamePos(true);
		for(int j(0); j<n; j++, it_neigh++){
			float const dist(it->distance(*it_neigh));
			if(i != j && dist < repulsionDistance){
				if(glm::any(glm::greaterThan(glm::abs(it->pos - it_neigh->pos), glm::vec3(FLT_EPSILON)))){
					tmp +=  glm::normalize(it->pos - it_neigh->pos)/(dist*dist);
					allSamePos = false;
				}
			}
			else
				if(i != j) allSamePos = false;
		}
		if(allSamePos)
			tmp += glm::ballRand(1.0f);
		it->accel += tmp * separationRate;
	}
}

void Flock::boxForce(){
	for(auto &b: boids){
		float const dist = (glm::length(b.pos)-box_size);
		if(glm::length(b.pos) > box_size){
			b.accel += wallRepulsionRate * (-glm::normalize(b.pos));
		}
	}
}

void Flock::speedRegulationForce(){
	float const naturalDecay = .05f;

	for(auto &b: boids){
		glm::vec3 tmp(0.0f);
		if(glm::all(glm::lessThan(glm::abs(b.speed), glm::vec3(FLT_EPSILON))))
			b.accel = glm::ballRand(10.0f);
		if(glm::length(b.speed) > Boid::v0)
			b.accel *= (1 - naturalDecay);
		else
			b.accel *= (1 + naturalDecay);
	}
}

void Flock::add_boid(){
	boids.push_back(Boid());
}

void Flock::init_boids(int nbBoids){
	for(int i(0); i<nbBoids; i++){
		Boid tmp;
		tmp.pos = glm::ballRand(10.0f);
		tmp.speed = glm::ballRand(Boid::v0);
		boids.push_back(tmp);
	}
}

unsigned int Flock::size(){
	return boids.size();
}

std::list<Boid>::iterator Flock::begin(){
	return boids.begin();
}

void Flock::update(){

	coherenceForce();
	repulsionForce();
	speedRegulationForce();
	boxForce();

	for(auto &b: boids){
		b.update(.15);
	}
	

}
