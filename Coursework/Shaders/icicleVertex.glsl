#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float dt;
uniform float scrollSpeed;
uniform float waterScale;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec2 texCoord;
in vec3 normal;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
} OUT;



void main(void) 
{    
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colour = colour;
	OUT.normal = normalize(mat3(modelMatrix) * normal);       
	gl_Position = vec4(position, 1.0); 
}
