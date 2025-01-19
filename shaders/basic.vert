#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;

void main() {
    vec4 wPosition = model * vec4(position, 1.0);
    gl_Position = projection * view * wPosition;
    fPosition = wPosition.xyz;
    fNormal = mat3(transpose(inverse(model))) * normal;
    fTexCoords = texCoords;
    fragPosLightSpace = lightSpaceTrMatrix * wPosition;
}
