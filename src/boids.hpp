#ifndef BOIDS_HPP
#define BOIDS_HPP

#include <list>

// GFLW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Boid{
	private:
	
	public:

		glm::vec3 pos;
		glm::vec3 dir;
		glm::vec3 orientation;
		glm::vec3 new_dir;
		float speed;

		Boid();
		float const distance(Boid const &b);

		void update(float time);

};

class Flock{
	private:
		//TODO: change names
		inline static float coherenceRate = .1f; //rad.s^-1
		inline static float separationRate = .4f; //rad.s^-1
		inline static float perceptionDistance = 1.0f; //units
		inline static float repulsionDistance = .2f; //units
		std::list<Boid> boids;
		
		float* const distances();
		void coherenceModifier(float* distance_array);
		void repulsionModifier(float* distance_array);

	public:
		void add_boid();
		unsigned int size();
		std::list<Boid>::iterator begin();
		
		void update(double lastTime);
};

#endif
