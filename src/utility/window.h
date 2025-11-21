#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class window {
    private:
    static void framebufferSizeCallback(GLFWwindow *win, int width, int heigth) {
        window* self = static_cast<window*>(glfwGetWindowUserPointer(win));

        self->width = width;
        self->heigth = heigth;

        glViewport(0, 0, width, heigth);
    }

    int width, heigth;
    
    GLFWwindow *currentWindow;
    public:
    
    window(int width, int heigth, const char *title) :  width(width), heigth(heigth) {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        currentWindow = glfwCreateWindow(width, heigth, title, NULL, NULL);
        if(currentWindow == NULL) {
            std::cout << "Could not open window, terminating GLFW..." << '\n';
            glfwTerminate();
        }
        glfwMakeContextCurrent(currentWindow);
        glfwSetFramebufferSizeCallback(currentWindow, framebufferSizeCallback);
        glfwSwapInterval(1);

        glfwSetWindowUserPointer(this->currentWindow, this);

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

    GLFWwindow* getwindow() { return this->currentWindow; }
    
    int getWidth() { return this->width; };
    int getHeigth() { return this->heigth; }
};

#endif