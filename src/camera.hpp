#ifndef MY_CAMERA_OPENGL_HPP
#define MY_CAMERA_OPENGL_HPP

#include <glm/glm.hpp>

class Window{
	private:
		GLFWwindow *window;
		int width, height;
	
	public:
		Window(int _width, int _height);
		Window();

		GLFWwindow* getaddr();
		void setWindow(GLFWwindow* _window);
		int getwidth();
		int getheight();
};

class Camera{
	private:
		Window window;
		glm::vec3 position;
		float hAngle;
		float vAngle;
		float Fov;
		float speed;
		float mouseSpeed;
		glm::mat4 view;
		glm::mat4 proj;

		double lastTime;

	public:
		Camera(Window _window);

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();

		void update();

};

#endif
