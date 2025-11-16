#version 330 core
#define MAX_OBJECTS 256
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int objectCount;
uniform vec3 objectPositions[MAX_OBJECTS];
uniform float objectMasses[MAX_OBJECTS];

float schwarzschildWell(float r, float mass) {
    mass *= 10.0;
    float softening = 15.0;
    float k = 0.4;
    float p = 0.5;
    return k * mass / pow(r*r + softening*softening, p);
}

void main() {
    vec3 pos = aPos;
    float yOffset = 0.0;

    for(int i = 0; i < objectCount; i++) {
        vec3 objXZ = vec3(objectPositions[i].x, 0.0, objectPositions[i].z);
        vec3 posXZ = vec3(pos.x, 0.0, pos.z);

        float r = length(objXZ - posXZ);

        float pull = schwarzschildWell(r, objectMasses[i]);

        yOffset += pull;
    }

    pos.y -= yOffset;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}