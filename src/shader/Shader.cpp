#include "Shader.h"

Shader &Shader::use() {
	glUseProgram(this->ID);
	return *this;
}

void Shader::compile(const char *vertexSource, const char *fragmentSource,
					 const char *geometrySource) {
	unsigned int sVertex, sFragment, gShader;
	// vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	// fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	if (geometrySource != nullptr) {
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(this->ID, gShader);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(gShader);
}

void Shader::setFloat(const char *name, float value) const {
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::setInteger(const char *name, int value) const {
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::setVector2f(const char *name, float x, float y) const {
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void Shader::setVector2f(const char *name, const glm::vec2 &value) const {
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void Shader::setVector3f(const char *name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void Shader::setVector3f(const char *name, const glm::vec3 &value) const {
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y,
				value.z);
}

void Shader::setVector4f(const char *name, float x, float y, float z,
						 float w) const {
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void Shader::setVector4f(const char *name, const glm::vec4 &value) const {
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z,
				value.w);
}

void Shader::setMatrix4(const char *name, const glm::mat4 &matrix) const {
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false,
					   glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, string type) {
	int success;
	char infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			cout << "ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				 << infoLog
				 << "\n -- --------------------------------------------------- "
					"-- "
				 << endl;
		}
	}

	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			cout << "ERROR::Shader: Link-time error: Type: " << type << "\n"
				 << infoLog
				 << "\n -- --------------------------------------------------- "
					"-- "
				 << endl;
		}
	}
}
