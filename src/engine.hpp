#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include "shader/ShaderManager.h"
#include "util/mouse.hpp"
#include "util/scene.hpp"
#include "util/sound.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <thread>

class Engine {
  private:
	GLFWwindow *window{};
	bool allowMove = true;
	float time_since_last_move = 999.0;
	float aspect = 0.0;
	int width = 1920;
	int height = 1080;
	float pulse = 0.0;
	bool countUp = false;
	const char *save_filename = "/res/settings.txt";
	glm::mat4 modelLeft{};	 // Model matrix for a 3D object
	glm::mat4 projection{}; // Orthographic projection matrix maps a 3D scene to a
	glm::mat4 view{};		 // The camera's position and orientation in the world

	// @note Call glCheckError() after every OpenGL call to check for errors.
	//   GLenum glCheckError_(const char *file, int line);
	//
	// @brief Macro for glCheckError_ function. Used for debugging.
	// #define glCheckError() glCheckError_(__FILE__, __LINE__)

	std::thread audioThread;
	std::atomic<bool> running{true};

  public:
	Engine();
	~Engine();

	float time = 0.00;
    std::unique_ptr<ShaderManager> shaderManager;
	std::unique_ptr<Mouse> mouse;
	Scene scene;
	Sound sound;
	Shader defaultShader;
	const glm::mat4 PROJECTION =
		glm::ortho(0.0f, static_cast<float>(this->width), 0.0f,
			  static_cast<float>(this->height), -1.0f, 1.0f);

	unsigned int initWindow(bool debug = false);
	void initShaders();
	void save();
	void initScene();
	void processInput();
	void initMatrices();
	void draw();
	void update();
	void render();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	glm::vec4 influences{0.0, 0.0, 0.0, 0.0};
	bool pulseUp = true;

	bool shouldClose();
};

#endif // GRAPHICS_ENGINE_H
