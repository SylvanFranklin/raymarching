#include "scene.hpp"

Scene::Scene() {
	this->indices = {0, 1, 2};
	this->vertices = {-1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, -1.0, 0.0,
					  -1.0, 1.0, 0.0, 1.0,	1.0,  0.0, 1.0, -1.0, 0.0};
}

void Scene::setShader(Shader &shader) { this->shader = shader; }

// Scene::Scene(Shader &shader){
//     this->shader = shader;
// };

void Scene::draw() {
	//	glBindVertexArray(VAO);
	//	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(this->VAO);
	//    glDrawArrays(GL_TRIANGLE_FAN, 0,
	//                 9);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//    glBindVertexArray(0);
}

void Scene::setUniforms(const glm::mat4 &model, const glm::mat4 &view,
						const glm::mat4 &projection, const glm::vec2 &mouse_pos,
						const float &aspect, bool &clicked, const float &time,
						const float &pulse) const {

	glm::mat4 modelMatrix = model;
	modelMatrix = translate(modelMatrix, glm::vec3(0.f, 0.f, 0.f));

	// The size of the shape is scaled by the model matrix to make the shape
	// larger or smaller.
	//    modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
	this->shader.setMatrix4("model", modelMatrix);
	this->shader.setMatrix4("view", view);
	this->shader.setMatrix4("projection", projection);
	this->shader.setVector2f("mouse", mouse_pos);
	this->shader.setFloat("aspect", aspect);
	this->shader.setFloat("time", time);
	this->shader.setFloat("pulse", pulse);
	this->shader.setInteger("clicked", clicked ? 1 : 0);
}

unsigned int Scene::initVAO() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	return VAO;
}

void Scene::initVBO() {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
				 vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
						  (void *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
