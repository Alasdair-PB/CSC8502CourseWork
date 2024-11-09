#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec2 texCoord;
in vec3 normal;
in vec4 tangent;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} OUT;

void main (void) 
{
    vec4 worldPos = modelMatrix * vec4(position, 1.0);

    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
    OUT.normal = normalize(mat3(modelMatrix) * normal);  
    OUT.tangent = normalize(mat3(modelMatrix) * tangent.xyz);  
    OUT.binormal = cross(OUT.normal, OUT.tangent); 
    OUT.worldPos = worldPos.xyz;

    gl_Position = projMatrix * viewMatrix * worldPos;
}
