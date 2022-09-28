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

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

//Boids
#include "boids.hpp"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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

GLuint compile_compute_shader(char const *compute_shader_path){
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
	//printf("code :\n%s\n", ComputeSourcePointer);
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
	GLuint computeForcesPrgm = glCreateProgram();
	printf("Linking program\n");
	glAttachShader(computeForcesPrgm, ComputeShaderID);
	glLinkProgram(computeForcesPrgm);

	glGetProgramiv(computeForcesPrgm, GL_LINK_STATUS, &Result);
	glGetProgramiv(computeForcesPrgm, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(computeForcesPrgm, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(computeForcesPrgm, ComputeShaderID);
	glDeleteShader(ComputeShaderID);

	return computeForcesPrgm;
}

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
	int const WIDTH = 1500, HEIGHT = 900;
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

	GLuint skyboxProgram = LoadShaders("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");

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

	glUseProgram(skyboxProgram);
	GLuint skyboxLoc = glGetUniformLocation(skyboxProgram, "skybox");
	glUniform1i(skyboxLoc, 0);

	//Set up compute shaders
	GLuint computeForcesPrgm = compile_compute_shader("src/shaders/ComputeNaiveShader.glsl");
	GLuint computeUpdateBoids = compile_compute_shader("src/shaders/ComputeUpdateBoids.glsl");

	

#define BOIDS 5000

	Boid flock[BOIDS];
	Boid mem_flock[BOIDS];
	for(int i(0); i<BOIDS; i++){
		flock[i].pos = glm::ballRand(10.0f);
		//if(i < 10) std::cout << tmp.pos.x << ";";
		flock[i].speed = glm::ballRand(v0);

		/*
		flock[i].pos[0] = tmp.pos[0];
		flock[i].pos[1] = tmp.pos[1];
		flock[i].pos[2] = tmp.pos[2];

		flock[i].speed[0] = tmp.speed[0];
		flock[i].speed[1] = tmp.speed[1];
		flock[i].speed[2] = tmp.speed[2];
		*/

		
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

	glUseProgram(computeForcesPrgm);
	GLint v0Loc = glGetUniformLocation(computeForcesPrgm, "v0");
	GLint maxVLoc = glGetUniformLocation(computeForcesPrgm, "maxV");
	GLint separationRateLoc = glGetUniformLocation(computeForcesPrgm, "separationRate");
	GLint wallRepulsionRateLoc = glGetUniformLocation(computeForcesPrgm, "wallRepulsionRate");
	GLint perceptionDistanceLoc = glGetUniformLocation(computeForcesPrgm, "perceptionDistance");
	GLint repulsionDistanceLoc = glGetUniformLocation(computeForcesPrgm, "repulsionDistance");
	GLint box_sizeLoc = glGetUniformLocation(computeForcesPrgm, "box_size");
	GLint coherenceRateLoc = glGetUniformLocation(computeForcesPrgm, "coherenceRate");
	GLint sizeLoc = glGetUniformLocation(computeForcesPrgm, "size");

	float v0 = 5.0f;
	float maxV = 7.0f;
	float separationRate = 25.0f;
	float wallRepulsionRate = 1.0f;
	float perceptionDistance = 10.0f;
	float repulsionDistance = 5.f;
	float box_size = 40.0f;
	float coherenceRate = .15f;


	glUniform1f(v0Loc, v0);
	glUniform1f(maxVLoc, maxV);
	glUniform1f(separationRateLoc, separationRate);
	glUniform1f(wallRepulsionRateLoc, wallRepulsionRate);
	glUniform1f(perceptionDistanceLoc, perceptionDistance);
	glUniform1f(repulsionDistanceLoc, repulsionDistance);
	glUniform1f(box_sizeLoc, box_size);
	glUniform1f(coherenceRateLoc, coherenceRate);
	glUniform1ui(sizeLoc, BOIDS);

	glUseProgram(computeUpdateBoids);
	v0Loc = glGetUniformLocation(computeUpdateBoids, "v0");
	maxVLoc = glGetUniformLocation(computeUpdateBoids, "maxV");
	GLint timeLoc = glGetUniformLocation(computeUpdateBoids, "time");
	sizeLoc = glGetUniformLocation(computeUpdateBoids, "size");


	float time = .09f;

	glUniform1f(v0Loc, v0);
	glUniform1f(maxVLoc, maxV);
	glUniform1f(timeLoc, time);
	glUniform1ui(sizeLoc, BOIDS);

	glEnable(GL_DEPTH_TEST);  
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Boid *gpuFlock = nullptr;


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
	GLuint block_index = glGetProgramResourceIndex(computeForcesPrgm, GL_SHADER_STORAGE_BLOCK, "boidBuffer");
	GLuint ssbo_binding_point_index = 2;
	glShaderStorageBlockBinding(computeForcesPrgm, block_index, ssbo_binding_point_index);
	*/

    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
		GLfloat timeValue = glfwGetTime();

		computeMatricesFromInputs();

			
		//Update flock
		glUseProgram(computeForcesPrgm);
			//1.Calculate barycenter
		glm::vec3 barycenter(0.0f);
		if(gpuFlock){
			for(int i(0); i<BOIDS; i++){
				barycenter.x += gpuFlock[i].pos[0];
				barycenter.y += gpuFlock[i].pos[1];
				barycenter.z += gpuFlock[i].pos[2];
			}
			barycenter = barycenter/(float) BOIDS;

			GLint barycenterLoc = glGetUniformLocation(computeForcesPrgm, "barycenter");
			glUniform3fv(barycenterLoc, 1, &barycenter[0]);
		}

		glDispatchCompute(groups, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
		gpuFlock = (Boid*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

		//std::cout << "****************************\n old : " << std::endl;
		//std::cout << gpuFlock[0].pos[0] << " " << gpuFlock[0].pos[1] << " " << gpuFlock[0].pos[2] << std::endl;
		//std::cout << gpuFlock[0].speed[0] << " " << gpuFlock[0].speed[1] << " " << gpuFlock[0].speed[2] << std::endl;
		//std::cout << gpuFlock[0].accel[0] << " " << gpuFlock[0].accel[1] << " " << gpuFlock[0].accel[2] << std::endl;

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		
		

		//glFinish();
		//GLfloat computeForcetime = glfwGetTime();
		//std::cout << "Compute Forces time : " << computeForcetime - timeValue << "\n";

		//std::cout << "azzadazazguy\n";
		/*
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
		gpuFlock = (Boid*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
		if(!gpuFlock){
			std::cout << "fuuuu\n";
			int error = glGetError();
			std::cout << "error : " << error << std::endl;
			std::cout << "GL_INVALID_ENUM : " << GL_INVALID_ENUM << std::endl;
			std::cout << "GL_INVALID_OPERATION : " << GL_INVALID_OPERATION << std::endl;
			std::cout << "GL_INVALID_VALUE : " << GL_INVALID_VALUE << std::endl;
		}
		for(int i(0); i<BOIDS; i++){
			
			//std::cout << "zaodhazuidz : " << i << std::endl;
			mem_flock[i] = Boid(gpuFlock[i]);
			//std::cout << mem_flock[i].pos[0] << " " << mem_flock[i].pos[1] << " " << mem_flock[i].pos[2] << std::endl;
			//std::cout << mem_flock[i].speed[0] << " " << mem_flock[i].speed[1] << " " << mem_flock[i].speed[2] << std::endl;
			//std::cout << mem_flock[i].accel[0] << " " << mem_flock[i].accel[1] << " " << mem_flock[i].accel[2] << std::endl;
			
			mem_flock[i].update(.05f); //Delta_T of simulation

			gpuFlock[i].pos = mem_flock[i].pos;
			gpuFlock[i].speed = mem_flock[i].speed;
			gpuFlock[i].accel = mem_flock[i].accel;
			//std::cout << "aft : " <<  mem_flock[i].speed[0] << " " << mem_flock[i].speed[1] << " " << mem_flock[i].speed[2] << std::endl;

		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		*/

		glUseProgram(computeUpdateBoids);
		glDispatchCompute(groups, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//GLfloat computeUpdatetime = glfwGetTime();
		//std::cout << "Compute Update time : " << computeUpdatetime - computeForcetime << "\n";


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boidsBuf);
		gpuFlock = (Boid*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

		memcpy(mem_flock, gpuFlock, sizeof(mem_flock));

		//std::cout << "new updated : " << std::endl;

		//std::cout << mem_flock[0].pos[0] << " " << mem_flock[0].pos[1] << " " << mem_flock[0].pos[2] << std::endl;
		//std::cout << mem_flock[0].speed[0] << " " << mem_flock[0].speed[1] << " " << mem_flock[0].speed[2] << std::endl;
		//std::cout << mem_flock[0].accel[0] << " " << mem_flock[0].accel[1] << " " << mem_flock[0].accel[2] << std::endl;

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		//GLfloat CopyToCPUtime = glfwGetTime();
		//std::cout << "Copy to CPU time : " << CopyToCPUtime - computeUpdatetime << "\n";
		//std::cout << "ok1 : " << std::endl;

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		//GLfloat offsetValue = (sin(timeValue) / 2);



		//glm::mat4 trans(1.0f);
		//trans = glm::rotate(trans, timeValue, glm::vec3(6.2f, 1.5f, .5f));

		glm::mat4 view = getViewMatrix();
		glm::mat4 projection = getProjectionMatrix();


        glUseProgram(shaderProgram);
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		//std::cout << "ok2 : " << std::endl;
		int count(0);
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
		//std::cout << "********* : " << count << std::endl;


		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		glUseProgram(skyboxProgram);
        view = glm::mat4(glm::mat3(getViewMatrix())); // remove translation from the view matrix
		viewLoc = glGetUniformLocation(skyboxProgram, "view");
		projectionLoc = glGetUniformLocation(skyboxProgram, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

		//std::cout << "Total shader time : " << glfwGetTime() - timeValue << "\n";
		//GLfloat Drawtime = glfwGetTime();
		//std::cout << "Draw time : " << Drawtime - CopyToCPUtime << "\n";

		// Swap the screen buffers
		glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(2, VAOs);
	glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, EBOs);
    glDeleteBuffers(1, &skyboxVBO);
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
