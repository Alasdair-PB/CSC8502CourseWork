#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;


layout (std140) uniform ObjectMatrices {
    mat4 matrices[500];
};

in vec2 texCoord;
in vec3 position;


out Vertex {
	vec2 texCoord;
} OUT;

void main (void) 
{
    mat4 vp = projMatrix * viewMatrix;
    gl_Position = vp * matrices[gl_InstanceID] * vec4(position, 1);
    OUT.texCoord = texCoord;
}
