#include "ShaderManager.h"
#include <fstream>
#include <sstream>

ShaderManager::~ShaderManager() { clear(); }

Shader ShaderManager::loadShader(const char *vShaderFile,
								 const char *fShaderFile,
								 const char *gShaderFile, std::string name) {
	return shaders[name] =
			   loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
}

Shader &ShaderManager::getShader(std::string name) { return shaders[name]; }

void ShaderManager::clear() {
	// delete all shader: "iter" here is const std::pair<std::string, Shader>&,
	// so we need to use "iter.second" to get the Shader, and delete the program
	// by ID
	for (const auto &iter : shaders)
		glDeleteProgram(iter.second.ID);
}

Shader ShaderManager::loadShaderFromFile(const char *vShaderFile,
										 const char *fShaderFile,
										 const char *gShaderFile) {
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try {
		// open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr) {
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	} catch (std::exception &e) {
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	const char *gShaderCode = geometryCode.c_str();
	// 2. now create shader object from source code
	Shader shader;
	shader.compile(vShaderCode, fShaderCode,
				   gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}
