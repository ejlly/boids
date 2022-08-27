// Include standard headers
#include <iostream>
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
	if(key == GLFW_KEY_N && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(key == GLFW_KEY_B && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

const GLchar *vertexCode = "#version 330 core\n\
							  layout(location = 0) in vec3 position;\n\
							  void main(){\n\
							  gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
							  }";

const GLchar *fragmentCode = "#version 330 core\n\
								out vec4 color;\n\
								uniform vec4 ourColor;\n\
								void main(){\n\
								color = ourColor;\n\
								}";

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
	//GLuint programID = LoadShaders("src/shaders/SimpleVertexShader.vertexshader", "src/shaders/SimpleFragmentShader.fragmentshader");
	//GLuint programID2 = LoadShaders("src/shaders/SimpleVertexShader.vertexshader", "src/shaders/SimpleFragmentShader2.fragmentshader");

	GLuint vertexProgram = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexProgram, 1, &vertexCode, nullptr);
	glCompileShader(vertexProgram);

	GLuint fragmentProgram = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentProgram, 1, &fragmentCode, nullptr);
	glCompileShader(fragmentProgram);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexProgram);
	glAttachShader(programID, fragmentProgram);
	glLinkProgram(programID);


	glDetachShader(programID, vertexProgram);
	glDetachShader(programID, fragmentProgram);
	
	glDeleteShader(vertexProgram);
	glDeleteShader(fragmentProgram);

	GLfloat vertices[] = {
	0.5f, 0.5f, 0.0f, // Top Right
	0.5f, -0.5f, 0.0f, // Bottom Right
	-0.5f, -0.5f, 0.0f, // Bottom Left
	-0.5f, 0.5f, 0.0f, // Top Left
	};

	GLuint indices[] = { // Note that we start from 0!
	0, 1, 3, // First Triangle
	1, 2, 3 // Second Triangle
	};

	GLfloat vertices2[] = {
	1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 0.0f, 0.0f
	};
	
	GLuint indices2[] = {
	0, 1, 2,
	1, 2, 3
	};

	GLuint VAOs[2], VBOs[2], EBOs[2];

	//First set

	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glGenBuffers(2, EBOs);

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Second set
	
	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

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

		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue * 4) / 2) + 0.5;
		GLfloat redValue = (sin(timeValue * 3) / 2) + 0.5;
		GLint vertexColorLocation = glGetUniformLocation(programID, "ourColor");

		glUseProgram(programID);
		//glUseProgram(vertexProgram);
		glUniform4f(vertexColorLocation, redValue, greenValue, 0.0f, 1.0f);

		// Use our shader
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);

		// Use our shader
		//glUseProgram(programID2);
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
		glBindVertexArray(0);
	
		// Swap buffers
		glfwSwapBuffers(window);

	}


	// Cleanup VBO
	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, EBOs);
	glDeleteBuffers(2, VBOs);

	glDeleteProgram(programID);
	//glDeleteProgram(programID2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

