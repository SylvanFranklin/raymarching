#ifndef UNIFORM_H
#define UNIFORM_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Uniform {
 public:
  unsigned int ID{};

  Uniform() {}
  Uniform &use();
  // compiles and links the UNIFORMs up
  void compile(const char *vertexSource, const char *fragmentSource,
               const char *geometrySource = nullptr);  // note: geometry source code is optional

  /// @brief set a uniform float in the UNIFORM
  /// @param name name of the uniform
  /// @param value float value to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setFloat(const char *name, float value) const;

  /// @brief set a uniform integer in the UNIFORM
  /// @param name name of the uniform
  /// @param value integer value to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setInteger(const char *name, int value) const;

  /// @brief set a uniform vector of two floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value x and y values to set as a glm::vec2
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector2f(const char *name, float x, float y) const;

  /// @brief set a uniform vector of two floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value glm::vec2 values to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector2f(const char *name, const glm::vec2 &value) const;

  /// @brief set a uniform vector of three floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value x, y and z values to set as a glm::vec3
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector3f(const char *name, float x, float y, float z) const;

  /// @brief set a uniform vector of three floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value glm::vec3 values to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector3f(const char *name, const glm::vec3 &value) const;

  /// @brief set a uniform vector of four floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value x, y, z and w values to set as a glm::vec4
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector4f(const char *name, float x, float y, float z, float w) const;

  /// @brief set a uniform vector of four floats in the UNIFORM
  /// @param name name of the uniform
  /// @param value glm::vec4 values to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setVector4f(const char *name, const glm::vec4 &value) const;

  /// @brief set a uniform matrix of four floats in the UNIFORM
  /// @param name name of the uniform
  /// @param matrix glm::mat4 values to set
  /// @param useUNIFORM boolean to indicate whether to use this uniform
  void setMatrix4(const char *name, const glm::mat4 &matrix) const;

 private:
  /// @brief Checks if compilation or linking failed and if so, print the
  /// error logs
  /// @param object the UNIFORM object to check
  /// @param type the type of UNIFORM object (vertex, fragment, geometry)
  void checkCompileErrors(unsigned int object, std::string type);

  std::string dumpConfig();
};

#endif
