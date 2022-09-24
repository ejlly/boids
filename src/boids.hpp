#ifndef BOIDS_HPP
#define BOIDS_HPP

//#include <list>

// GFLW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/random.hpp>

float anglef(glm::vec3 const &a, glm::vec3 const &b);

struct GpuBoid{
	glm::vec3 pos;
	float pad1;
	glm::vec3 speed;
	float pad2;
	glm::vec3 accel;
	float pad3;
};


inline static glm::vec3 const asset_orientation = glm::vec3(0.0f, 0.f, 1.0f);

inline static float const v0 = 5.0f;
inline static float const maxV = 7.0f;

class Boid{
	private:
	
	public:

		alignas(16) glm::vec3 pos;
		alignas(16) glm::vec3 speed;
		alignas(16) glm::vec3 accel;


		Boid();
		Boid(Boid &tmpboid);
		Boid(GpuBoid tmpboid);
		float const distance(Boid const &b);
		glm::vec3 const dir();

		void update(float time);

		void get_model(glm::mat4 &model);

};

#define MAX_BOIDS 10000

class Flock{
	private:
		//TODO: change names
		inline static float separationRate = 13.f;
		inline static float wallRepulsionRate = 1.0f;  
		inline static float perceptionDistance = 15.0f; //units
		inline static float repulsionDistance = 1.f; //units
		inline static float box_size = 60.0f; //must be bigger than 1.0f
		Boid boids[MAX_BOIDS];
		unsigned int m_size;
		
		void coherenceForce();
		void repulsionForce();
		void boxForce();
		void speedRegulationForce();

	public:
		inline static float coherenceRate = 0.14f;

		bool add_boid();
		void init_boids(unsigned int nbBoids);
		unsigned int size();
		
		void update();

		Boid& operator[](int i);
};

#endif
