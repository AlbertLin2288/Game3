#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "utils.h"


char infoLog[512];

bool compileShader(GLuint &shader, GLenum type, const char* source){
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        return 0;
    };
    return 1;
}

Shader::Shader() {
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // read
    std::string vertexCode;
    std::string fragmentCode;
    try {
        vertexCode = readFile(vertexPath);
        fragmentCode = readFile(fragmentPath);
    } catch(std::ifstream::failure e) {
        printf("Error: Failed to read shader file\n");
        exit(1);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // compile
    GLuint vertex, fragment;
    if (!compileShader(vertex, GL_VERTEX_SHADER, vShaderCode)){
        printf("Error: compiling vertex shader failed.\n%s\n",infoLog);
        exit(1);
    }
    if (!compileShader(fragment, GL_FRAGMENT_SHADER, fShaderCode)){
        printf("Error: compiling fragment shader failed.\n%s\n",infoLog);
        exit(1);
    }

    // link
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    // print linking errors if any
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Error: linking shader program failed.\n%s\n", infoLog);
        exit(1);
    }
    // delete shaders; they’re linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(program);
}

GLint Shader::getUniformLocation(const std::string &name) const {
    return glGetUniformLocation(program, name.c_str());
}

void Shader::setUniform(const GLint location, int value) const {
    glUniform1i(location, value);
}

void Shader::setUniform(const GLint location, float value) const {
    glUniform1f(location, value);
}

void Shader::setUniform(const GLint location, const glm::vec3 &value) const {
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const GLint location, const glm::vec4 &value) const {
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const GLint location, const glm::mat4 &value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setVec4f(const std::string &name, const glm::vec4& vec) const {
    glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setMat4f(const std::string &name, const glm::mat4& mat) const {
    glUniformMatrix4fv(
        glGetUniformLocation(program, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(mat)
    );
}