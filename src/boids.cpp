#include "boids.hpp"

Boid::Boid(){
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	dir = glm::vec3(1.0f, 0.0f, 0.0f);
	orientation = dir;
	new_dir = glm::vec3(0.0f, 0.0f, 0.0f);
	speed = .5f;
}

float const Boid::distance(Boid const &b){
	return glm::distance(b.pos, pos);
}

void Boid::update(float time){
	pos = pos + 12.f*time*speed*dir;
	//rotate dir into new_dir
	dir += .02f * new_dir;
	new_dir = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::normalize(dir);
}

float* const Flock::distances(){
	int const n = boids.size();
	//all other items are distances
	float* distance_array = new float[(n*(n+1))/2];

	std::list<Boid>::iterator it1(boids.begin()), it2;

	for(int i(0); i<n; i++, it1++){
		it2 = it1++; it1--;
		for(int j(i+1); j<n; j++, it2++){
			distance_array[(i*(i+1))/2 + j - 1] = it1->distance(*it2);
		}
	}

	return distance_array;
	//TODO: delete after use
}

void Flock::coherenceModifier(float* distance_array){
	int const n = boids.size();
	
	auto it(boids.begin());

	glm::vec3 bary(0.0f, 0.0f, 0.0f);

	for(auto b: boids){
		bary += b.dir;
	}

	for(int i(0); i<n; i++, it++){
		auto it_neigh(boids.begin());
		int count = 0;
		glm::vec3 tmp(it->dir);
		for(int j(0); j<n; j++, it_neigh){
			if(i < j)
				if(distance_array[(i*(i+1))/2 + j - 1] < perceptionDistance){
					tmp += it_neigh->dir;
					count++;
				}
			if(i > j)
				if(distance_array[(j*(j+1))/2 + i - 1] < perceptionDistance){
					tmp += it_neigh->dir;
					count++;
				}
		}
		if(count > 0)
			it->new_dir += tmp/(float) count;
		else //no one near
			it->new_dir += bary/(float) n;

		it->new_dir *= coherenceRate;
	}
}

void Flock::repulsionModifier(float* distance_array){
	int const n = boids.size();
	
	auto it(boids.begin());

	for(int i(0); i<n; i++, it++){
		auto it_neigh(boids.begin());
		int count = 0;
		glm::vec3 tmp(it->dir);
		for(int j(0); j<n; j++, it_neigh){
			if(i < j)
				if(distance_array[(i*(i+1))/2 + j - 1] < repulsionDistance){
					tmp += glm::cross(it->dir + eps_vector, it_neigh->dir);
					count++;
				}
			if(i > j)
				if(distance_array[(j*(j+1))/2 + i - 1] < repulsionDistance){
					tmp += glm::cross(it->dir + eps_vector, it_neigh->dir);
					count++;
				}
		}
		if(count > 0)
			it->new_dir += tmp/(float) count;
		
		it->new_dir *= separationRate;
	}
}

void Flock::add_boid(){
	boids.push_back(Boid());
}

unsigned int Flock::size(){
	return boids.size();
}

std::list<Boid>::iterator Flock::begin(){
	//TODO: fail if list is empty ?
	return boids.begin();
}

void Flock::update(double lastTime){
	double currentTime = glfwGetTime();

	float* distance_array = distances();
	coherenceModifier(distance_array);
	repulsionModifier(distance_array);
	
	delete[] distance_array;

	for(auto &b: boids){
		b.update(currentTime - lastTime);
	}

	lastTime = currentTime;
}
