#ifndef SCENE_HPP
#define SCENE_HPP

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "../shader/Shader.hpp"

class Scene {
 private:
  unsigned int VBO{};
  unsigned int VAO{};
  std::vector<unsigned int> indices;
  std::vector<float> vertices;

 public:
  void setShader(Shader &shader);
  Shader shader;
  Scene();
  void initVBO();
  unsigned int initVAO();
  void draw();
  void setUniforms(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection,
                   const glm::vec2 &mouse_pos, const float &aspect, bool &clicked, const float &time,
                   const float &pulse, const float &level) const;
};

#endif  // SCENE_HPP
