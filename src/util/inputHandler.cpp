#include "inputHandler.hpp"
#include <algorithm>

InputHandler::InputHandler(GLFWwindow *window) { this->window = window; }

//void InputHandler::update_keys() {
//	std::copy(std::begin(keys), std::end(keys), std::begin(last_keys));
//	for (int key = 0; key < 1024; ++key) {
//		if (glfwGetKey(window, key) == GLFW_PRESS)
//			keys[key] = true;
//		else if (glfwGetKey(window, key) == GLFW_RELEASE)
//			keys[key] = false;
//	}
//}
void InputHandler::update_mouse() {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// we do this to not stap the camera around
	if (this->firstMouse) {
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		this->firstMouse = false;
	}
	MouseXOffset = mouseX - lastMouseX;
	MouseYOffset = mouseY - lastMouseY;
	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

glm::vec2 InputHandler::get_mouse_pos() {
	return glm::vec2(lastMouseX, lastMouseY);
}
