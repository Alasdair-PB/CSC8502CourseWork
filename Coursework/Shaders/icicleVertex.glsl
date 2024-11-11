#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float dt;
uniform float scrollSpeed;
uniform float waterScale;

in vec3 position;
in vec4 colour;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;

void main(void) 
{
    OUT.texCoord = texCoord;
	OUT.colour = colour;
	gl_Position = vec4(position, 1.0); 
}
