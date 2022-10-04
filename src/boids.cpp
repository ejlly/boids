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
	speed = tmpboid.speed;
	accel = tmpboid.accel;
}

Boid::Boid(GpuBoid tmpboid){
	pos = tmpboid.pos;
	speed = tmpboid.speed;
	accel = tmpboid.accel;
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
