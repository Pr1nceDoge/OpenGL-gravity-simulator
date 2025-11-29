#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera {
private:
    static inline glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    static inline glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

    static inline glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    static void updateCameraVectors() {
        glm::vec3 front;

        front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        front.y = sin(glm::radians(cameraPitch));
        front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

        cameraFront = glm::normalize(front);
        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
    }

    static void processCursorInput(GLFWwindow*, double xOffset, double yOffset) {
        static bool firstMouse = true;
        static float lastX, lastY;
        
        if(cursorEnabled) {
            firstMouse = true;
            return;
        }

        if(firstMouse) {
            lastX = xOffset;
            lastY = yOffset;
            firstMouse = false;
        }

        float xPos = (lastX - xOffset) * sensitivity;
        float yPos = (lastY - yOffset) * sensitivity;
        lastX = xOffset;
        lastY = yOffset;

        cameraYaw -= xPos;
        cameraPitch += yPos;

        cameraPitch = std::min(cameraPitch, 89.0f);
        cameraPitch = std::max(cameraPitch, -89.0f);

        updateCameraVectors();
    }

    static void processScrollInput(GLFWwindow*, double, double yOffset) {
        cameraPosition += cameraFront * scrollSpeed * (float)yOffset;
    }

    static inline bool cursorEnabled = false;

    static inline float cameraYaw = 90.0f, cameraPitch = 0.0f;
public:
    static inline glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -100.0f);

    static inline float cameraSpeed = 50.0f;
    static inline float scrollSpeed = 25.0f;
    static inline float sensitivity = 0.15f;

    static glm::mat4 getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    }

    static glm::vec3 getCameraPosition() {
        return cameraPosition;
    }

    static void processKeyboardInput(GLFWwindow *window, float deltaTime) {
        float velocity = cameraSpeed * deltaTime;
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? velocity /= 4 : velocity;

        if(glfwGetKey(window, GLFW_KEY_W)) {
            cameraPosition += cameraFront * velocity;
        }
        else if(glfwGetKey(window, GLFW_KEY_S)) {
            cameraPosition -= cameraFront * velocity;
        }
        
        if(glfwGetKey(window, GLFW_KEY_A)) {
            cameraPosition -= cameraRight * velocity;
        }
        else if(glfwGetKey(window, GLFW_KEY_D)) {
            cameraPosition += cameraRight * velocity;
        }

        if(glfwGetKey(window, GLFW_KEY_E)) {
            cameraPosition += cameraUp * velocity;
        }
        else if(glfwGetKey(window, GLFW_KEY_Q)) {
            cameraPosition -= cameraUp * velocity;
        }
    }

    static void toogleCursor(GLFWwindow* window) {
        cursorEnabled = !cursorEnabled;
        
        if(cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    static void processCursorCallback(GLFWwindow* window) {
        cursorEnabled ? glfwSetCursorPosCallback(window, nullptr) : glfwSetCursorPosCallback(window, processCursorInput);
    }

    static void processScrollCallback(GLFWwindow* window) {
        glfwSetScrollCallback(window, processScrollInput);
    }
};