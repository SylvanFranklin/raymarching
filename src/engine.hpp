#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <complex>
#include <glm/glm.hpp>
#include <memory>
#include <thread>

#include "shader/ShaderManager.h"
#include "util/mouse.hpp"
#include "util/scene.hpp"
#include "util/sound.hpp"

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
  glm::mat4 modelLeft{};   // Model matrix for a 3D object
  glm::mat4 projection{};  // Orthographic projection matrix maps a 3D scene to a
  glm::mat4 view{};        // The camera's position and orientation in the world

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
  std::unique_ptr<Sound> sound;
  Shader defaultShader;
  glm::vec4 frequencies = glm::vec4(0.0);
  const glm::mat4 PROJECTION =
      glm::ortho(0.0f, static_cast<float>(this->width), 0.0f, static_cast<float>(this->height), -0.0001f, 1.0f);

  unsigned int initWindow(bool debug = false);
  void initShaders();
  void save();
  void initScene();
  void processInput();
  void initMatrices();
  void draw();
  void update();
  void render();
  bool shouldClose();

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  glm::vec4 influences{0.0, 0.0, 0.0, 0.0};
  bool pulseUp = true;

  struct AudioDebugComponent {
    float scale = 1.0f;
    bool isShown = false;
  } audioDebugComponent;

  struct AudioState {
    std::vector<float> buffer;
    std::vector<std::complex<float>> fftOutput;
    float level;
  } audioState;
};

#endif  // GRAPHICS_ENGINE_H
