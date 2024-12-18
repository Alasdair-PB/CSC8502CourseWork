#version 400 core

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
