#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform mat4 textureMatrix;
uniform vec4 nodeColour;
uniform float dt;
uniform float scrollSpeed;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;


out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} OUT;

void main(void) {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.texCoord.x = texCoord.x * (mod(dt, 1.0)); // * dt;
    OUT.texCoord.y = texCoord.y;



    OUT.normal = normalize(normalMatrix * normalize(normal));

    vec4 worldPos = modelMatrix * vec4(position, 1);
    OUT.worldPos = worldPos.xyz;

    gl_Position = projMatrix * viewMatrix * worldPos;

    OUT.colour = nodeColour;
}
