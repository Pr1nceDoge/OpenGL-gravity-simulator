#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class shader {
    public:
    GLuint ID;

    shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vertexShaderFile;
        std::ifstream fragmentShaderFile;

        vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vertexShaderFile.open(vertexShaderPath);
            fragmentShaderFile.open(fragmentShaderPath);
            std::stringstream vertexShaderStream, fragmentShaderStream;

            vertexShaderStream << vertexShaderFile.rdbuf();
            fragmentShaderStream << fragmentShaderFile.rdbuf();

            vertexShaderFile.close();
            fragmentShaderFile.close();

            vertexCode = vertexShaderStream.str();
            fragmentCode = fragmentShaderStream.str();

        }
        catch (std::ifstream::failure &error) {
            std::cout << "Could not read shader file: " << error.what() << '\n';
        }
        const char *vertexShaderCode = vertexCode.c_str();
        const char *fragmentShaderCode = fragmentCode.c_str();

        GLuint vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShaderCode, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "Could not compile VERTEX shader: " << infoLog << '\n';
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "Could not compile FRAGMENT shader: " << infoLog << '\n';
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);

        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Could not link shaders: " << infoLog << '\n';
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() {
        glUseProgram(ID);
    }

    void setBool(const GLchar *name, bool value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void setInt(const GLchar *name, int value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void setFloat(const GLchar *name, float value) {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void setVec4(const GLchar *name, glm::vec4 value) {
        glUniform4fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(value));
    }

    void setVec3(const GLchar *name, glm::vec3 value) {
        glUniform3fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(value));
    }

    void setVec2(const GLchar *name, glm::vec2 value) {
        glUniform2fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(value));
    }

    void setVec1(const GLchar *name, glm::vec1 value) {
        glUniform1fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(value));
    }

    void setMat4(const GLchar *name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setMat3(const GLchar *name, glm::mat3 value) {
        glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setMat2(const GLchar *name, glm::mat2 value) {
        glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }
};