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

	/*
	if(glm::any(glm::greaterThan(glm::abs(steerTwds), glm::vec3(FLT_EPSILON))) && glm::any(glm::greaterThan(glm::abs(speed), glm::vec3(FLT_EPSILON)))){
		glm::mat4 steering(1.0f);
		if(glm::any(glm::greaterThan(glm::abs(speed - steerTwds), glm::vec3(FLT_EPSILON)))){

			auto const a1 = -glm::acos(glm::dot(dir, steerTwds)) * Flock::coherenceRate;
			auto const a2 = glm::cross(speed, steerTwds);

			steering = glm::rotate(steering, a1, a2);
		
			//std::cout << "angle : " << a1 << std::endl;
			//std::cout << "rot around : " << a2[0] << " " << a2[1] << " " << a2[2] << std::endl;

			//std::cout << steering[0][0] << " " << steering[0][1] << " " << steering[0][2] << std::endl << steering[1][0] << " " << steering[1][1] << " " << steering[1][2] << std::endl << steering[2][0] << " " << steering[2][1] << " " << steering[2][2] << std::endl << std::endl;
	}
	

	speed = glm::mat3(steering) * speed;
	}
	*/

	if(glm::length(speed) > maxV)
		speed = dir*maxV;


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
	
	//std::cout << model[0][0] << " " << model[0][1] << " " << model[0][2] << std::endl << model[1][0] << " " << model[1][1] << " " << model[1][2] << std::endl << model[2][0] << " " << model[2][1] << " " << model[2][2] << std::endl << std::endl;
}

void Flock::coherenceForce(){
	//TODO: modify this repulsion into coherence
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
				//std::cout << "dist : " << dist << " & repulsion dist : " << repulsionDistance << std::endl;
				//std::cout << "modi\n";
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
				//std::cout << "dist : " << dist << " & repulsion dist : " << repulsionDistance << std::endl;
				if(glm::any(glm::greaterThan(glm::abs(it->pos - it_neigh->pos), glm::vec3(FLT_EPSILON)))){
					//std::cout << "modi\n";
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
	//TODO : delete elements out of the box, with a small margin to make sure they don't "jump" out of the box
	
	float const dist_to_box = 5.0f;


	for(auto &b: boids){
		glm::vec3 tmp(0.0f);
		for(int dimension(0); dimension<3; dimension++){
			if(glm::abs(b.pos[dimension] - box_size) < dist_to_box){
				glm::vec3 wallRepulsionForce(0.0f);
				wallRepulsionForce[dimension] = -1/glm::abs(b.pos[dimension] - box_size);
				tmp += wallRepulsionForce;
			}
			else if(glm::abs(b.pos[dimension] + box_size) < dist_to_box){
				glm::vec3 wallRepulsionForce(0.0f);
				
				wallRepulsionForce[dimension] = 1/glm::abs(b.pos[dimension] - box_size);
				tmp += wallRepulsionForce;
			}
		}
		b.accel = b.accel * .5f +  wallRepulsionRate * tmp;
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
		boids.push_back(tmp);
	}
}

unsigned int Flock::size(){
	return boids.size();
}

std::list<Boid>::iterator Flock::begin(){
	//TODO: fail if list is empty ?
	return boids.begin();
}

void Flock::update(){

	coherenceForce();
	repulsionForce();
	boxForce();
	speedRegulationForce();

	for(auto &b: boids){
		b.update(.15);
	}
	

}
