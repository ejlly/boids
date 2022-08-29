#ifndef BOIDS_HPP
#define BOIDS_HPP

#include <list>

// GFLW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/random.hpp>

float anglef(glm::vec3 const &a, glm::vec3 const &b);

class Boid{
	private:
		inline static glm::vec3 const asset_orientation = glm::vec3(0.0f, 0.f, 1.0f);
	
	public:

		glm::vec3 pos;
		glm::vec3 speed;
		glm::vec3 accel;

		glm::vec3 new_dir;

		Boid();
		float const distance(Boid const &b);
		glm::vec3 const dir();

		void update(float time);

		void get_model(glm::mat4 &model);

};

class Flock{
	private:
		//TODO: change names
		inline static float coherenceRate = .1f; //rad.s^-1
		inline static float separationRate = .4f; //rad.s^-1
		inline static float perceptionDistance = 5.0f; //units
		inline static float repulsionDistance = 2.f; //units
		inline static glm::vec3 eps_vector = glm::vec3(1e-6, 1e-6, 1e-6);
		inline static float box_size = 30.0f;
		std::list<Boid> boids;
		
		void coherenceModifier(float* distance_array);
		void repulsionModifier();

	public:
		void add_boid();
		void init_boids(int nbBoids);
		unsigned int size();
		std::list<Boid>::iterator begin();
		
		void update();
};

#endif
