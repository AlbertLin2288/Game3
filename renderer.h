#pragma once

#include <tuple>
#include <glm/fwd.hpp>
#include "shader.h"

class Window {
    float aspect_ratio;
    int width, height;
    GLFWwindow* window;
public:
    Window(int a_width=800, int a_height=600);
    ~Window();
    void swapBuffers();
    void pollEvents();
    int getKey(int key) const;
    bool windowShouldClose() const;
    double getTime() const;
    void fill(float r, float g, float b, float a) const;
    float get_aspect_ratio();
    std::pair<int, int> get_size();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
private:
};

class Circle {
    Shader shader;
    GLuint EBO, VAO, VBO;
    GLint modelLoc, viewProjLoc, colorLoc;
public:
    Circle();
    ~Circle();
    void draw(const glm::mat4 &model, const glm::mat4 &viewProj, const glm::vec4 &color) const;
};

class Triangle {
    Shader shader;
    GLuint EBO, VAO, VBO;
    GLint modelLoc, viewProjLoc, colorLoc;
public:
    Triangle();
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);
    ~Triangle();
    void draw(const glm::mat4 &model, const glm::mat4 &viewProj, const glm::vec4 &color) const;
};