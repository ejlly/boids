#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "keys.hpp"
#include "camera.hpp"


#include <cstdio>

Window::Window(int _width, int _height){
	width = _width;
	height = _height;

    //glfwMakeContextCurrent(window);

}

Window::Window(){
	Window(1500, 900);
}

GLFWwindow* Window::getaddr(){
	return window;
}


void Window::setWindow(GLFWwindow* _window){
	window = _window;
}

int Window::getwidth(){
	return width;
}

int Window::getheight(){
	return height;
}

Camera::Camera(Window _window){
	position = glm::vec3(0, 0, 200);
	hAngle = 3.14f;
	vAngle = 0.0f;
	Fov = 70.0f;
	speed = 30.0f;
	mouseSpeed = 0.005f;
	
	window = _window;

	lastTime = glfwGetTime();
}

glm::mat4 Camera::getViewMatrix(){
	return view;
}

glm::mat4 Camera::getProjectionMatrix(){
	return proj;
}

void Camera::update(){
	double curTime = glfwGetTime();
	float dt = (float) (curTime - lastTime);
	lastTime = curTime;

	int const width(window.getwidth()), height(window.getheight());
	GLFWwindow* win(window.getaddr());

	double xpos, ypos;
	glfwGetCursorPos(win, &xpos, &ypos);
	glfwSetCursorPos(win, width/2, height/2);

	hAngle += mouseSpeed * (float) (width/2 - xpos);
	vAngle += mouseSpeed * (float) (height/2 - ypos);

	glm::vec3 direction(cos(vAngle) * sin(hAngle), sin(vAngle), cos(vAngle) * cos(hAngle));

	glm::vec3 right(sin(hAngle - 3.14f/2.0f), 0, cos(hAngle - 3.14f/2.0f));

	glm::vec3 up = glm::cross(right, direction);

	if(glfwGetKey(win, GLFW_KEY_UP ) == GLFW_PRESS) position += direction * dt * speed;
	if(glfwGetKey(win, GLFW_KEY_DOWN ) == GLFW_PRESS) position -= direction * dt * speed;
	if(glfwGetKey(win, GLFW_KEY_RIGHT ) == GLFW_PRESS) position += right * dt * speed;
	if(glfwGetKey(win, GLFW_KEY_LEFT ) == GLFW_PRESS) position -= right * dt * speed;

	proj = glm::perspective(glm::radians(Fov), 4.0f/3.0f, .1f, 1000.0f);
	view = glm::lookAt(position, position+direction, up);
}
