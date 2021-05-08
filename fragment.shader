#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragmentPos;
in vec3 normal;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(lightPos - fragmentPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPos - fragmentPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, lightDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // phong
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0);
}