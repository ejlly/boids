#include "boids.hpp"
#include <iostream>

float anglef(glm::vec3 const &a, glm::vec3 const &b){
	//a and b are already normalized
	return glm::acos(glm::dot(a, b));
}

Boid::Boid(){
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	speed = glm::vec3(0.0f, 0.0f, 0.0f);
	new_dir = glm::vec3(0.0f, 0.0f, 0.0f);
	accel = glm::vec3(0.0f, 0.0f, 0.0f);
}

float const Boid::distance(Boid const &b){
	return glm::distance(b.pos, pos);
}

glm::vec3 const Boid::dir(){
	return glm::normalize(speed);
}

void Boid::update(float time){
	
	float const naturalDecay = .99995f;
	speed += time*accel;
	speed *= naturalDecay;

	pos += time*speed;

	//might need to born values of speed
	/*
	glm::mat4 model(1.0f);

	glm::vec3 rotVector = glm::normalize(glm::cross(dir, new_dir));
	if(!glm::isnan(rotVector[0]) && !glm::isnan(rotVector[1]) && !glm::isnan(rotVector[2])){
		
		model = glm::rotate(model, glm::acos(glm::dot(dir, new_dir))/2, rotVector);

		dir = glm::mat3(model) * dir;
		std::cout << "dir is : " << dir[0] << " " << dir[1] << " " << dir[2] << std::endl;
	std::cout << "new_dir is : " << new_dir[0] << " " << new_dir[1] << " " << new_dir[2] << std::endl;
	}
	else{
		std::cout << "Failed to rotate !\n";
		std::cout << "new_dir is : " << new_dir[0] << " " << new_dir[1] << " " << new_dir[2] << std::endl;
	}

		std::cout << model[0][0] << " " << model[0][1] << " " << model[0][2] << std::endl << model[1][0] << " " << model[1][1] << " " << model[1][2] << std::endl << model[2][0] << " " << model[2][1] << " " << model[2][2] << std::endl << std::endl;


	new_dir = glm::vec3(0.0f, 0.0f, 0.0f);
	*/

	accel = glm::vec3(0.0f);
}

void Boid::get_model(glm::mat4 &model){
	model = glm::translate(model, -pos);
	glm::vec3 dir = glm::normalize(speed);
	if(glm::any(glm::greaterThan(glm::abs(dir - asset_orientation), glm::vec3(FLT_EPSILON))))
		model = glm::rotate(model, glm::acos(glm::dot(dir, asset_orientation)), glm::cross(dir, asset_orientation));
}

void Flock::coherenceModifier(float* distance_array){
	/*
	int const n = boids.size();
	
	auto it(boids.begin());

	glm::vec3 bary(0.0f, 0.0f, 0.0f);

	for(auto b: boids){
	}

	for(int i(0); i<n; i++, it++){
		auto it_neigh(boids.begin());
		glm::vec3 const dir = it->dir();
		bary += dir;
		int count = 0;
		glm::vec3 tmp(dir);
		for(int j(0); j<n; j++, it_neigh){
			if((i < j && distance_array[(i*(i+1))/2 + j - 1] < repulsionDistance) || 
			   (i > j && distance_array[(j*(j+1))/2 + i - 1] < repulsionDistance)){
					//tmp += it_neigh->dir();
					count++;
				}
		}
		if(count > 0){
			it->new_dir += tmp * coherenceRate;
			std::cout << "coherend\n";
		}
		else //no one near
			it->new_dir += bary/(float) n;
	}
	*/
}

void Flock::repulsionModifier(){
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

void Flock::boxModifier(){
	//TODO : delete elements out of the box, with a small margin to make sure they don't "jump" out of the box
	
	float const dist_to_box = 4.0f;


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
		b.accel += 10.0f * wallRepulsionRate * tmp;
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

	repulsionModifier();
	boxModifier();

	for(auto &b: boids){
		b.update(.15);
	}
	

}
