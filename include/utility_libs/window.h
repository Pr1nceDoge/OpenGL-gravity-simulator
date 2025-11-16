#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class window {
    private:
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    public:
    GLFWwindow *currentWindow;
    
    window(const int width, const int height, const char *title ) {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        currentWindow = glfwCreateWindow(width, height, title, NULL, NULL);
        if(currentWindow == NULL) {
            std::cout << "Could not open window, terminating GLFW..." << '\n';
            glfwTerminate();
        }
        glfwMakeContextCurrent(currentWindow);
        glfwSetFramebufferSizeCallback(currentWindow, framebufferSizeCallback);
        glfwSwapInterval(1);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Could not load GLAD, terminating GLFW..." << '\n';
            glfwTerminate();
        }
    }

    bool windowShouldClose() {
        return glfwWindowShouldClose(currentWindow);
    }

    void swapBuffers() {
        glfwSwapBuffers(currentWindow);
    }
};


#endif