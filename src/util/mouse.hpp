#ifndef Mouse_H
#define Mouse_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Mouse {

  public:
	Mouse(GLFWwindow *window);
	void update();
	double sensitivity = 100;
	double offsetX, offsetY, deltaX, deltaY;
	bool clicked = false;
	glm::vec2 get_pos();
	glm::vec2 get_delta();

  private:
	double lastX, lastY;
	bool firstMove = true;
	GLFWwindow *window;
};

#endif // Mouse_H
