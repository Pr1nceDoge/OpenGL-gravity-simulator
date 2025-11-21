#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utility/window.h"
#include "utility/shader.h"
#include "utility/camera.h"

class object {
private:
    GLuint VAO, VBO, EBO;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    shader &objectShader;

    const int stacks = 64;
    const int sectors = 64;
public:
    glm::vec3 position{};
    glm::vec3 acceleration{};
    glm::vec3 velocity{};

    const float mass;
    const float density;
    const GLfloat radius;

    const bool star;
    const glm::vec3 color;

    object(glm::vec3 position, glm::vec3 velocity, float mass, float density, glm::vec3 color, bool star, shader &objectShader):
    mass(mass),
    density(density),
    radius(std::cbrt((3.0f * this->mass) / (4.0f * M_PI * this->density))),
    color(color),
    star(star),
    objectShader(objectShader) {

        this->position = position;
        this->velocity = velocity;

        for(int i = 0; i <= stacks; i++) {
            float phi = M_PI * i / stacks;

            for(int j = 0; j <= sectors; j++) {
                float theta = 2.0f * M_PI * j / sectors;

                GLfloat x = radius * sin(phi) * cos(theta);
                GLfloat y = radius * sin(phi) * sin(theta);
                GLfloat z = radius * cos(phi);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                vertices.push_back(x / radius);
                vertices.push_back(y / radius);
                vertices.push_back(z / radius);
            }
        }

        for(int i = 0; i < stacks; i++) {
            for(int j = 0; j < stacks; j++) {
                int first = i * (sectors + 1) + j;
                int second = first + sectors + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, std::vector<object*> stars) {
        objectShader.use();
        glm::mat4 modelMatrix = glm::translate(model, this->position);

        objectShader.setMat4("model", modelMatrix);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        objectShader.setVec3("color", color);
        for(int i = 0; i < stars.size(); i++) {
            std::string baseLine = "pointLights[" + std::to_string(i) + ']';

            objectShader.setVec3((baseLine + ".position").c_str(), stars[i]->position);
            objectShader.setVec3((baseLine + ".ambient").c_str(), stars[i]->color * 0.1f);
            objectShader.setVec3((baseLine + ".diffuse").c_str(), glm::vec3(0.5f));

            if(i >= 4) break;
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void applyForce(const glm::vec3 &force) {
        this->acceleration += force / this->mass;
    }

    void updatePosition(const float &deltaTime) {
        this->velocity += this->acceleration * deltaTime;
        this->position += this->velocity * deltaTime;

        this->acceleration = glm::vec3(0.0f);
    }

    ~object() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};

class grid {
private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint VAO, VBO, EBO;
    shader &gridShader;

    const int N_DIVS;
    const int N_LINES;
    const float STEP;

    void createVertices(int size) {
        for(int i = 0; i <= N_DIVS; i++) {
            float z = (float)i - (float)N_LINES;

            for(int j = 0; j <= N_DIVS; j++) {
                float x = (float)j - (float)N_LINES;

                vertices.push_back(x * STEP);
                vertices.push_back(0.0f); 
                vertices.push_back(z * STEP);
            }
        }

        int pointsPerLine = N_DIVS + 1;
        for(int i = 0; i <= N_DIVS; i++) {
            for(int j = 0; j < N_DIVS; j++) {
                indices.push_back(i * pointsPerLine + j);
                indices.push_back(i * pointsPerLine + j + 1);
            }
        }

        for (int j = 0; j <= N_DIVS; j++) {
            for (int i = 0; i < N_DIVS; i++) {
                indices.push_back(i * pointsPerLine + j);
                indices.push_back((i + 1) * pointsPerLine + j);
            }
        }
    }
public:
    grid(int size, float step, shader &gridShader) : N_LINES(size), N_DIVS(size * 2), STEP(step), gridShader(gridShader) {
        createVertices(size);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, std::vector<object*> objects) {
        gridShader.use();

        gridShader.setMat4("model", model);
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);
        
        gridShader.setInt("objectCount", objects.size());
        for(int i = 0; i < objects.size(); i++) {
            gridShader.setVec3(("objectPositions[" + std::to_string(i) + ']').c_str(), objects[i]->position);
            gridShader.setFloat(("objectMasses[" + std::to_string(i) + ']').c_str(), objects[i]->mass);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    ~grid() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

class physicsEngine {
private:
    static inline std::vector<std::unique_ptr<object>> objects;
    static inline std::vector<object*> objectPointers;
    static inline std::vector<object*> stars;

    static constexpr float GRAVITY = 100.0f;
    static constexpr float EPS = 0.01f;
public:
    template<typename... Args>
    static void addObject(Args&&... args) {
        objects.emplace_back(std::make_unique<object>(std::forward<Args>(args)...));
    }

    static auto& getObjects() {
        objectPointers.clear();

        for(auto& obj : objects) {
            objectPointers.emplace_back(obj.get());
        }
        return objectPointers; 
    }

    static auto& getStars() {
        stars.clear();

        for(auto& obj : objects) {
            if(obj->star == true) stars.emplace_back(obj.get());
        }
        return stars; 
    }

    static void updatePhysics(const float &deltaTime) {
        for(size_t i = 0; i < objects.size(); i++) {

            for(size_t j = i + 1; j < objects.size(); j++) {
                glm::vec3 direction = objects[j]->position - objects[i]->position;

                float distance = glm::dot(direction, direction) + EPS * EPS;
                if(distance < 0.01f) continue;

                float forceMagnitude = GRAVITY * (objects[j]->mass * objects[i]->mass) / distance;

                glm::vec3 directionNormalized(glm::normalize(direction));
                glm::vec3 force = forceMagnitude * directionNormalized;

                objects[i]->applyForce(force);
                objects[j]->applyForce(-force);
            }
        }

        for (auto& obj : objects) {
            obj->updatePosition(deltaTime);
        }
    }
};

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGTH = 600;

void checkCursor(GLFWwindow *window);
float getDeltaTime();
int main() {
    window myWindow(WINDOW_WIDTH, WINDOW_HEIGTH, "Planetary simulation");
    glfwSetInputMode(myWindow.getwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera::cameraSpeed = 75.0f;
    camera::scrollSpeed = 32.5f;

    camera::processCursorCallback(myWindow.getwindow());
    camera::processScrollCallback(myWindow.getwindow());

    shader defaultShader("src/shaders/default_shader.vert", "src/shaders/default_shader.frag"); // for the planets
    shader lightingShader("src/shaders/lighting_shader.vert", "src/shaders/lighting_shader.frag"); // for the stars
    shader gridShader("src/shaders/grid_shader.vert", "src/shaders/grid_shader.frag"); // for the grid

    grid newGrid(200, 5.0f, gridShader);

    physicsEngine::addObject(
        glm::vec3(100.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 15.0f),
        5.0f,
        0.5f,
        glm::vec3(0.54f, 0.89f, 1.0f),
        false,
        lightingShader
    );

    physicsEngine::addObject(
        glm::vec3(-150.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -20.0f),
        3.0f,
        0.1f,
        glm::vec3(0.94f, 0.34f, 0.07f),
        false,
        lightingShader
    );

    physicsEngine::addObject(
        glm::vec3(250.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 15.0f),
        10.0f,
        0.05f,
        glm::vec3(0.82f, 0.3f, 0.3f),
        false,
        lightingShader
    );

    physicsEngine::addObject(
        glm::vec3(275.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 20.0f),
        0.5f,
        0.1f,
        glm::vec3(0.77f ,0.78f ,0.73f),
        false,
        lightingShader
    );

    physicsEngine::addObject(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        500.0f,
        0.05f,
        glm::vec3(1.0f, 0.9f, 0.6f),
        true,
        defaultShader
    );

    while(!myWindow.windowShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float deltaTime = getDeltaTime();

        camera::processKeyboardInput(myWindow.getwindow(), deltaTime);
        checkCursor(myWindow.getwindow());

        glm::mat4 view = camera::getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)myWindow.getWidth() / (float)myWindow.getHeigth(), 0.1f, 75000.0f);

        physicsEngine::updatePhysics(deltaTime);
        for(auto &obj : physicsEngine::getObjects()) {
            obj->draw(glm::mat4(1.0f), view, projection, physicsEngine::getStars());
        }
        newGrid.draw(glm::mat4(1.0f), view, projection, physicsEngine::getObjects());

        myWindow.swapBuffers();
        glfwPollEvents();
    }

    return 0;
}

void checkCursor(GLFWwindow *window) {
    static bool keyPressed = false;
    bool escapeDown = glfwGetKey(window, GLFW_KEY_ESCAPE);

    if(escapeDown && !keyPressed) {
        camera::toogleCursor(window);
    }
    keyPressed = escapeDown;
}

float getDeltaTime() {
    static float lastFrameTime{};
    static float deltaTime{};

    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    return deltaTime;
}