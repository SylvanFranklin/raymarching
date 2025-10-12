#include "mouse.hpp"

Mouse::Mouse(GLFWwindow *window) { this->window = window; }

void Mouse::update() {
  double mouseX, mouseY;
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS) {
    this->clicked = true;
  } else {
    this->clicked = false;
  }

  glfwGetCursorPos(window, &mouseX, &mouseY);
  if (this->firstMove) {
    lastX = mouseX;
    lastY = mouseY;
    this->firstMove = false;
  }
  offsetX = mouseX - lastX;
  offsetY = mouseY - lastY;
  lastX = mouseX;
  lastY = mouseY;
  deltaX = offsetX / sensitivity;
  deltaY = offsetY / sensitivity;
}

glm::vec2 Mouse::get_pos() { return glm::vec2(lastX, lastY); }
