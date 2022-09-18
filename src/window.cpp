#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <sstream>

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

//
#include "common/shader.hpp"
#include "common/controls.hpp"

//Boids
#include "boids.hpp"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLFWwindow* window;

// The MAIN function, from here we start the application and run the game loop
int main(){

    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
	int const WIDTH = 1024, HEIGHT = 768;
    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    // Build and compile our shader program
	GLuint shaderProgram = LoadShaders("src/shaders/SimpleVertexShader.vs", "src/shaders/SimpleFragmentShader.fs");
	


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

#define BOIDS 1000

	/*

	Flock flock;
	flock.init_boids(BOIDS);

	*/
	
	GpuBoid flock[BOIDS];
	Boid mem_flock[BOIDS];
	for(int i(0); i<BOIDS; i++){
		Boid tmp;
		tmp.pos = glm::ballRand(10.0f);
		//if(i < 10) std::cout << tmp.pos.x << ";";
		tmp.speed = glm::ballRand(Boid::v0);
		tmp.speed = glm::vec3(.00001f);
		tmp.accel = glm::vec3(1.0f);

		flock[i].pos[0] = tmp.pos[0];
		flock[i].pos[1] = tmp.pos[1];
		flock[i].pos[2] = tmp.pos[2];

		flock[i].speed[0] = tmp.speed[0];
		flock[i].speed[1] = tmp.speed[1];
		flock[i].speed[2] = tmp.speed[2];

		if(i < 10) std::cout << tmp.accel.x << ";";

		flock[i].accel[0] = .0f;
		flock[i].accel[1] = .0f;
		flock[i].accel[2] = .0f;

	}
	
	std::cout << std::endl;

	char const compute_shader_path[] = "src/shaders/ComputeNaiveShader.glsl";

	//Compiling Computing shader
	GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	std::string ComputeShaderCode;
	std::ifstream ComputeShaderStream(compute_shader_path, std::ios::in);
	if(ComputeShaderStream.is_open()){
		std::stringstream sstr;
		sstr << ComputeShaderStream.rdbuf();
		ComputeShaderCode = sstr.str();
		ComputeShaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", compute_shader_path);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	printf("Compiling shader : %s\n", compute_shader_path);
	char const * ComputeSourcePointer = ComputeShaderCode.c_str();
	glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, nullptr);
	glCompileShader(ComputeShaderID);

	// Check Vertex Shader
	glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0 ){
		std::vector<char> ComputeShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(ComputeShaderID, InfoLogLength, nullptr, &ComputeShaderErrorMessage[0]);
		printf("%s\n", &ComputeShaderErrorMessage[0]);
	}



	//Linking program
	GLuint ComputePrgm = glCreateProgram();
	printf("Linking program\n");
	glAttachShader(ComputePrgm, ComputeShaderID);
	glLinkProgram(ComputeShaderID);

	glGetProgramiv(ComputePrgm, GL_LINK_STATUS, &Result);
	glGetProgramiv(ComputePrgm, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ComputePrgm, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ComputePrgm, ComputeShaderID);
	glDeleteShader(ComputeShaderID);



	//Boids buffer
	GLuint boidsBuf;
	glGenBuffers(1, &boidsBuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, boidsBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BOIDS * sizeof(GpuBoid), flock, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	
	glUseProgram(ComputePrgm);
	GLuint nbUnits = BOIDS/1024;
	GLuint groups = 1;
	if(nbUnits > groups) groups = nbUnits;

	GLint v0Loc = glGetUniformLocation(ComputePrgm, "v0");
	GLint maxVLoc = glGetUniformLocation(ComputePrgm, "maxV");
	GLint separationRateLoc = glGetUniformLocation(ComputePrgm, "separationRate");
	GLint wallRepulsionRateLoc = glGetUniformLocation(ComputePrgm, "wallRepulsionRate");
	GLint perceptionDistanceLoc = glGetUniformLocation(ComputePrgm, "perceptionDistance");
	GLint repulsionDistanceLoc = glGetUniformLocation(ComputePrgm, "repulsionDistance");
	GLint box_sizeLoc = glGetUniformLocation(ComputePrgm, "box_size");
	GLint coherenceRateLoc = glGetUniformLocation(ComputePrgm, "coherenceRate");
	GLint sizeLoc = glGetUniformLocation(ComputePrgm, "size");

	glUniform1f(v0Loc, 5.0f);
	glUniform1f(maxVLoc, 7.0f);
	glUniform1f(separationRateLoc, 13.f);
	glUniform1f(wallRepulsionRateLoc, 1.0f);
	glUniform1f(perceptionDistanceLoc, 15.0f);
	glUniform1f(repulsionDistanceLoc, 1.f);
	glUniform1f(box_sizeLoc, 60.0f);
	glUniform1f(coherenceRateLoc, .14f);
	glUniform1ui(sizeLoc, BOIDS);


	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float *gpuFlock = nullptr;

	std::cout << "OK : in game loop" << std::endl;

	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group counts x:%i y:%i z:%i\n",
	  work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
	  work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	

	GLint work_grp_inv = 0;

	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);



	//Find where binding is : 
	/*
	GLuint block_index = glGetProgramResourceIndex(ComputePrgm, GL_SHADER_STORAGE_BLOCK, "boidBuffer");
	GLuint ssbo_binding_point_index = 2;
	glShaderStorageBlockBinding(ComputePrgm, block_index, ssbo_binding_point_index);
	*/

    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
		GLfloat timeValue = glfwGetTime();

		computeMatricesFromInputs();
		
		//std::cout << "uguy\n";
		//Update flock
		//flock.update();
		glUseProgram(ComputePrgm);
			//1.Calculate barycenter
		glm::vec3 barycenter(0.0f);
		if(gpuFlock){
			for(int i(0); i<BOIDS; i++){
			//	barycenter.x += gpuFlock[i].pos[0];
			//	barycenter.y += gpuFlock[i].pos[1];
			//	barycenter.z += gpuFlock[i].pos[2];
			}
			barycenter = barycenter/(float) BOIDS;

			GLint barycenterLoc = glGetUniformLocation(ComputePrgm, "barycenter");
			glUniform3fv(barycenterLoc, 1, &barycenter[0]);
		}

		glDispatchCompute(2, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		GLsync fenceSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, NULL);

		glClientWaitSync(fenceSync, 0, 0);

		//std::cout << "azzadazazguy\n";
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
		gpuFlock = (float*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
		if(!gpuFlock){
			std::cout << "fuuuu\n";
			int error = glGetError();
			std::cout << "error : " << error << std::endl;
			std::cout << "GL_INVALID_ENUM : " << GL_INVALID_ENUM << std::endl;
			std::cout << "GL_INVALID_OPERATION : " << GL_INVALID_OPERATION << std::endl;
			std::cout << "GL_INVALID_VALUE : " << GL_INVALID_VALUE << std::endl;
		}
		for(int i(0); i<BOIDS; i++){
			
			std::cout << gpuFlock[9*i + 0] << ";";
			std::cout << gpuFlock[9*i + 1] << ";";
			std::cout << gpuFlock[9*i + 2] << ";";
			std::cout << gpuFlock[9*i + 3] << ";";
			std::cout << gpuFlock[9*i + 4] << ";";
			std::cout << gpuFlock[9*i + 5] << ";";
			std::cout << gpuFlock[9*i + 6] << ";";
			std::cout << gpuFlock[9*i + 7] << ";";
			std::cout << gpuFlock[9*i + 8] << ";";
			std::cout << std::endl;
			//std::cout << "zaodhazuidz : " << i << std::endl;
			/*
			mem_flock[i] = Boid();
			
			
			mem_flock[i].update(.05f); //Delta_T of simulation

			gpuFlock[i].pos[0] = mem_flock[i].pos[0];
			gpuFlock[i].pos[1] = mem_flock[i].pos[1];
			gpuFlock[i].pos[2] = mem_flock[i].pos[2];

			gpuFlock[i].speed[0] = mem_flock[i].speed[0];
			gpuFlock[i].speed[1] = mem_flock[i].speed[1];
			gpuFlock[i].speed[2] = mem_flock[i].speed[2];

			gpuFlock[i].accel[0] = mem_flock[i].accel[0];
			gpuFlock[i].accel[1] = mem_flock[i].accel[1];
			gpuFlock[i].accel[2] = mem_flock[i].accel[2];
			*/
			//std::cout << mem_flock[i].speed[0] << " " << mem_flock[i].speed[1] << " " << mem_flock[i].speed[2] << std::endl;

		}
		//glUnmapNamedBuffer(boidsBuf);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		std::cout << "ok1 : " << std::endl;

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        glUseProgram(shaderProgram);
		
		GLfloat offsetValue = (sin(timeValue) / 2);



		//glm::mat4 trans(1.0f);
		//trans = glm::rotate(trans, timeValue, glm::vec3(6.2f, 1.5f, .5f));

		glm::mat4 view = getViewMatrix();
		glm::mat4 projection = getProjectionMatrix();


		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		//std::cout << "ok2 : " << std::endl;
		for(int i(0); i<BOIDS; i++){

			//std::cout << cur.accel[0] << " " << cur.accel[1] << " " << cur.accel[2] << std::endl;

			glm::mat4 model(1.0f);
			mem_flock[i].get_model(model);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//Draw the structure
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindVertexArray(VAOs[1]);
			glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		//std::cout << "ok3 : " << std::endl;

		// Swap the screen buffers
		glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, EBOs);
	glDeleteBuffers(1, &boidsBuf);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if(key == GLFW_KEY_N && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(key == GLFW_KEY_B && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
