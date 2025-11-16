#version 330 core
#define MAX_POINT_LIGHTS 4

out vec4 fragColor;
in vec3 fragPos;
in vec3 normal;

struct light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
};

uniform light pointLights[MAX_POINT_LIGHTS];
uniform vec3 viewerPos;
uniform vec3 color;

vec3 calculatePointLight(light pointLight, vec3 norm) {
    vec3 lightDir = normalize(pointLight.position - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    float dist = length(pointLight.position - fragPos);

    vec3 ambient = pointLight.ambient * color;
    vec3 diffuse = pointLight.diffuse * diff * color;

    return ambient + diffuse;
}

void main() {
    vec3 norm = normalize(normal);

    vec3 result = vec3(0.0);
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += calculatePointLight(pointLights[i], norm); 
    }

    fragColor = vec4(result, 1.0);
}