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
		inline static float const v0 = 0.5f;
		inline static float const maxV = 5.0f;

		glm::vec3 pos;
		glm::vec3 speed;
		glm::vec3 accel;

		glm::vec3 steerTwds;


		Boid();
		float const distance(Boid const &b);
		glm::vec3 const dir();

		void update(float time);

		void get_model(glm::mat4 &model);

};

class Flock{
	private:
		//TODO: change names
		inline static float separationRate = .4f;
		inline static float wallRepulsionRate = 10*separationRate;  
		inline static float perceptionDistance = 10.0f; //units
		inline static float repulsionDistance = 1.f; //units
		inline static float box_size = 30.0f; //must be bigger than 1.0f
		std::list<Boid> boids;
		
		void coherenceForce();
		void repulsionForce();
		void boxForce();
		void speedRegulationForce();

	public:
		inline static float coherenceRate = 0.04f;

		void add_boid();
		void init_boids(int nbBoids);
		unsigned int size();
		std::list<Boid>::iterator begin();
		
		void update();
};

#endif
