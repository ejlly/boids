#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <string.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

//Boids
#include "boids.hpp"
#include "keys.hpp"
#include "shader_progs.hpp"
#include "camera.hpp"

// Function prototypes

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

//GLFWwindow* window;

// The MAIN function, from here we start the application and run the game loop
int main(){

    glfwInit();
	
	int const WIDTH = 1500, HEIGHT = 900;
	Window win(WIDTH, HEIGHT);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow *window(win.getaddr());

    window = glfwCreateWindow(WIDTH, HEIGHT, "Boids Simulation", nullptr, nullptr);
	win.setWindow(window);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WIDTH, HEIGHT);

	Camera cam(win);

	DrawingProgram shaderProgram("src/shaders/SimpleVertexShader.vs", "src/shaders/SimpleFragmentShader.fs");
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
		//Position				//Color Face			//Color edge		
		-.5f,	-.5f,	-.5f,	.0f,	.75f,	.5f,	.0f,	.0f,	.0f, 
		-.5f,	.5f,	-.5f,	.0f,	.75f,	.5f,	.0f,	.0f,	.0f,
		.5f,	.5f,	-.5f,	.0f,	.75f,	.5f,	.0f,	.0f,	.0f,
		.5f,	-.5f,	-.5f,	.0f,	.75f,	.5f,	.0f,	.0f,	.0f,
		0.0f,	0.0f,	1.0f,	1.0f,	.5f,	.0f,	.0f,	.0f,	.0f,
    };


	GLuint indices[] = {
		0,	1,	2,
		0,	2,	3,
		0,	1,	4,
		1,	2,	4,
		2,	3,	4,
		3,	0,	4
	};

	GLuint edge_indices[] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		0, 4,
		1, 4,
		2, 4,
		3, 4
	};


    GLuint EBOs[2], VBO, VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(1, &VBO);
	glGenBuffers(2, EBOs);


	//Polyedra
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO


	//Edges
    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edge_indices), edge_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (GLvoid*) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0); // Unbind VAO

	//Set up Sky Box textures
	std::vector<std::string> faces = {
		"src/skybox/Daylight Box_Right.bmp",
		"src/skybox/Daylight Box_Left.bmp",
		"src/skybox/Daylight Box_Top.bmp",
		"src/skybox/Daylight Box_Bottom.bmp",
		"src/skybox/Daylight Box_Front.bmp",
		"src/skybox/Daylight Box_Back.bmp"
	};

	unsigned int cubemapTexture = loadCubemap(faces); 

	DrawingProgram skyboxProgram("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	skyboxProgram.use();
	skyboxProgram.uniformi("skybox", 0);

	//Set up compute shaders
	ComputeProgram computeForces("src/shaders/ComputeNaiveShader.glsl");
	ComputeProgram computeUpdateBoids("src/shaders/ComputeUpdateBoids.glsl");

	

#define BOIDS 5000

	Boid flock[BOIDS];
	Boid mem_flock[BOIDS];
	for(int i(0); i<BOIDS; i++){
		flock[i].pos = glm::ballRand(10.0f);
		flock[i].speed = glm::ballRand(v0);
	}

	//Boids buffer
	GLuint boidsBuf;
	glGenBuffers(1, &boidsBuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, boidsBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BOIDS * sizeof(Boid), flock, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	
	GLuint nbUnits = BOIDS/32;
	GLuint groups = 1;
	if(nbUnits >= groups){
		groups = nbUnits+1;
	}

	std::cout << "nbGroups : " << groups << std::endl;

	computeForces.use();

	float v0 = 5.0f;
	float maxV = 7.0f;
	float separationRate = 25.0f;
	float wallRepulsionRate = 1.0f;
	float perceptionDistance = 10.0f;
	float repulsionDistance = 15.f;
	float box_size = 40.0f;
	float coherenceRate = .15f;

	computeForces.uniformf("v0", v0);
	computeForces.uniformf("maxV", maxV);
	computeForces.uniformf("separationRate", separationRate);
	computeForces.uniformf("wallRepulsionRate", wallRepulsionRate);
	computeForces.uniformf("perceptionDistance", perceptionDistance);
	computeForces.uniformf("repulsionDistance", repulsionDistance);
	computeForces.uniformf("box_size", box_size);
	computeForces.uniformf("coherenceRate", coherenceRate);
	computeForces.uniformui("size", BOIDS);


	computeUpdateBoids.use();

	float time = .06f;

	computeUpdateBoids.uniformf("v0", v0);
	computeUpdateBoids.uniformf("maxV", maxV);
	computeUpdateBoids.uniformf("time", time);
	computeUpdateBoids.uniformui("size", BOIDS);

	glEnable(GL_DEPTH_TEST);  
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Boid *gpuFlock = nullptr;


    // Game loop
    while (!glfwWindowShouldClose(win.getaddr())){
        glfwPollEvents();
		GLfloat timeValue = glfwGetTime();

		//computeMatricesFromInputs();
		cam.update();

			
		//Update flock
		computeForces.use();
		//1.Calculate barycenter
		glm::vec3 barycenter(0.0f);
		if(gpuFlock){
			for(int i(0); i<BOIDS; i++){
				barycenter += gpuFlock[i].pos[0];
			}
			barycenter = barycenter/(float) BOIDS;
			computeForces.uniform_3f("barycenter", 1, &barycenter[0]);
		}

		glDispatchCompute(groups, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		computeUpdateBoids.use();
		glDispatchCompute(groups, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
		gpuFlock = (Boid*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

		memcpy(mem_flock, gpuFlock, sizeof(mem_flock));

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glm::mat4 view = getViewMatrix();
		//glm::mat4 projection = getProjectionMatrix();

		shaderProgram.use();
		shaderProgram.uniform_4x4("view", 1, GL_FALSE, glm::value_ptr(cam.getViewMatrix()));
		shaderProgram.uniform_4x4("projection", 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));

		int count(0);
		for(int i(0); i<BOIDS; i++){
			glm::mat4 model(1.0f);
			mem_flock[i].get_model(model);

			shaderProgram.uniform_4x4("model", 1, GL_FALSE, glm::value_ptr(model));
			//Draw the structure
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindVertexArray(VAOs[1]);
			glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		glDepthFunc(GL_LEQUAL); 
		skyboxProgram.use();
		glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
		
		skyboxProgram.uniform_4x4("view", 1, GL_FALSE, glm::value_ptr(view));
		skyboxProgram.uniform_4x4("projection", 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

		// Swap the screen buffers
		glfwSwapBuffers(win.getaddr());
    }
    glDeleteVertexArrays(2, VAOs);
	glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, EBOs);
    glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &boidsBuf);

    glfwTerminate();
    return 0;
}
