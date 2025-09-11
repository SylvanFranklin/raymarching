#ifndef SCENE_HPP
#define SCENE_HPP

#include "../shader/Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

using std::vector;

class Scene {
  private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	vector<unsigned int> indices;
	vector<float> vertices;

  public:
	void setShader(Shader &shader);
	Shader shader;
	Scene();
	void initVBO();
	unsigned int initVAO();
	//    void initEBO();
	void draw();
	void setUniforms(const glm::mat4 &model, const glm::mat4 &view,
					 const glm::mat4 &projection, const glm::vec2 &mouse_pos,
					 const float &aspect, bool &clicked, const float &time,
					 const float &pulse) const;
};

#endif // SCENE_HPP
