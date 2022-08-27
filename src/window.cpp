// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>

#include "common/shader.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
// When a user presses the escape key, we set the WindowShouldCloseproperty to true,
// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(){
	/*
	mat4 myMatrix;
	vec4 myVector;
	// fill myMatrix and myVector somehow
	vec4 transformedVector = myMatrix * myVector; // Yeah, it's pretty much the same than GLM

	glm::mat4 myMatrix;
	glm::vec4 myVector;
	// fill myMatrix and myVector somehow
	glm::vec4 transformedVector = myMatrix * myVector; // Again, in this order ! this is important.
	*/


	// Initialise GLFW
	if(!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLuint const width = 1024, height = 768;
	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
	if(window == nullptr){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	int w_width, w_height;
    glfwGetFramebufferSize(window, &w_width, &w_height);  
    glViewport(0, 0, w_width, w_height);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("src/shaders/SimpleVertexShader.vertexshader", "src/shaders/SimpleFragmentShader.fragmentshader" );


	static const GLfloat g_vertex_buffer_data[] = { 
		-.5f, -.5f, 0.0f,
		 .5f, -.5f, 0.0f,
		 0.0f,  .5f, 0.0f,
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	// 1rst attribute buffer : vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glfwSetKeyCallback(window, key_callback);

	// Check if the ESC key was pressed or the window was closed
	//while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window)){
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glBindVertexArray(VAO);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glBindVertexArray(0);

		// Swap buffers
		glfwSwapBuffers(window);

	}


	// Cleanup VBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

