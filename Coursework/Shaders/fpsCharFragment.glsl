#version 330 core

uniform sampler2D diffuseTex;  
uniform sampler2D icicleMask;  
uniform vec3 cameraPos;   

in Vertex {
    vec2 texCoord;
} IN;

out vec4[2] fragColour; 

void main(void) 
{
    vec4 diffuse = texture(diffuseTex, IN.texCoord);  
    diffuse.z = 1;
    fragColour[1] = vec4(1);
}
