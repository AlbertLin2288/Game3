#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

Window::Window(int a_width, int a_height) : width(a_width), height(a_height) {

    #ifdef __linux
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    #endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // window
    aspect_ratio = (float)width / (float)height;

    window = glfwCreateWindow(width, height, "Game3", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        exit(1);
    }

    glViewport(0, 0, width, height);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

Window::~Window() {
    glfwTerminate();
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

int Window::getKey(int key) const {
    return glfwGetKey(window, key);
}

bool Window::windowShouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::fill(GLfloat r, GLfloat g, GLfloat b, GLfloat a) const {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

double Window::getTime() const {
    return glfwGetTime() * 1000;
}

float Window::get_aspect_ratio() {
    return aspect_ratio;
}

std::pair<int, int> Window::get_size() {
    return {width, height};
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    Window *obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    obj->width = width;
    obj->height = height;
    obj->aspect_ratio = (float)width / (float)height;
}

const float CIRCLE_VERTICES[] = {
    -1.0f, -1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f,
     1.0f, -1.0f, 1.0f,
     1.0f,  1.0f, 1.0f,
};

const unsigned int CIRCLE_INDICES[] = {
    0, 1, 2,
    1, 2, 3,
};

Circle::Circle() {
    shader = Shader("../shaders/circleShader.vs", "../shaders/circleShader.fs");

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CIRCLE_VERTICES), CIRCLE_VERTICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CIRCLE_INDICES), CIRCLE_INDICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    modelLoc = shader.getUniformLocation("model");
    viewProjLoc = shader.getUniformLocation("viewProj");
    colorLoc = shader.getUniformLocation("ourColor");
}

Circle::~Circle() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Circle::draw(const glm::mat4 &model, const glm::mat4 &viewProj, const glm::vec4 &color) const {
    shader.use();
    shader.setUniform(modelLoc, model);
    shader.setUniform(viewProjLoc, viewProj);
    shader.setUniform(colorLoc, color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}