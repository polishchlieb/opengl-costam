#version 330 core

layout(location = 0) out vec4 color;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D textureID;

in vec3 fragmentPos;
in vec2 texCoord;
in vec3 normal;

void main() {
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(lightPos - fragmentPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

    // specular
    vec3 viewDirection = normalize(viewPos - fragmentPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, lightDirection), 0.0), material.shininess);
    vec3 specular = (spec * material.specular) * light.specular;

    // color from texture
    vec4 sampled = texture(textureID, texCoord);

    // phong
    vec4 phong = vec4(ambient + diffuse + specular, 1.0);

    color = phong * sampled;
}