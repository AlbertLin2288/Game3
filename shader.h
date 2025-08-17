#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <string>

#include "myvec.h"

class Shader {
public:
    GLuint program;
    // constructor reads and builds the shader
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use() const;
    // utility uniform functions
    GLint getUniformLocation(const std::string &name) const;

    void setUniform(const GLint location, int value) const;
    void setUniform(const GLint location, float value) const;
    void setUniform(const GLint location, const myvec::vec2 &value) const;
    void setUniform(const GLint location, const glm::vec3 &value) const;
    void setUniform(const GLint location, const myvec::vec3 &value) const;
    void setUniform(const GLint location, const glm::vec4 &value) const;
    void setUniform(const GLint location, const glm::mat4 &value) const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2f(const std::string &name, const glm::vec2 &value) const;
    void setVec4f(const std::string &name, const glm::vec4& vec) const;
    void setMat4f(const std::string &name, const glm::mat4& mat) const;
};
