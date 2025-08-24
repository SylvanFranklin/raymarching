#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputHandler {

  public:
	InputHandler(GLFWwindow *window);
	void update_mouse();
//	void update_keys();
	bool key_pressed(int key) { return keys[key]; }
	bool key_pressed_released(int key) { return !keys[key] && last_keys[key]; }
	double MouseXOffset, MouseYOffset;
	glm::vec2 get_mouse_pos();

  private:
	bool keys[1024];
	bool last_keys[1024];
	double lastMouseX, lastMouseY;
	GLFWwindow *window;
	bool firstMouse = true;
};

#endif // INPUTHANDLER_H
