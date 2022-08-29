#include <iostream>
#include <stdlib.h>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
	GLuint shaderProgram = LoadShaders("src/shaders/SimpleVertexShader.vertexshader", "src/shaders/SimpleFragmentShader.fragmentshader");
	


    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
		//Position				//Color Face			//Color edge
		-.5f,	-.5f,	-.5f,	.0f,	.5f,	.5f,	.0f,	.0f,	.0f,
		-.5f,	.5f,	-.5f,	.0f,	.5f,	.5f,	.0f,	.0f,	.0f,
		.5f,	.5f,	-.5f,	.0f,	.5f,	.5f,	.0f,	.0f,	.0f,
		.5f,	-.5f,	-.5f,	.0f,	.5f,	.5f,	.0f,	.0f,	.0f,
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


#define BOIDS 200

	Flock flock;
	flock.init_boids(BOIDS);

	

	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	
	glEnable(GL_DEPTH_TEST);  

    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
		GLfloat timeValue = glfwGetTime();

		computeMatricesFromInputs();
		flock.update();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        glUseProgram(shaderProgram);
		
		GLfloat offsetValue = (sin(timeValue) / 2);



		//glm::mat4 trans(1.0f);
		//trans = glm::rotate(trans, timeValue, glm::vec3(6.2f, 1.5f, .5f));

		glm::mat4 view = getViewMatrix();
		glm::mat4 projection = getProjectionMatrix();



		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		auto it = flock.begin();
		for(int i(0); i<flock.size(); i++, it++){

			glm::mat4 model(1.0f);
			it->get_model(model);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//Draw the structure
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindVertexArray(VAOs[1]);
			glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, EBOs);
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
