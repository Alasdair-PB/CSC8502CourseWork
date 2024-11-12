#version 330 core
uniform sampler2D diffuseTex;  

in Vertex {
    vec2 texCoord;
} IN;

out vec4[2] fragColour; 

void main(void) 
{
    vec4 diffuse = texture(diffuseTex, IN.texCoord);  
    diffuse.a = 1;
    fragColour[0] = diffuse;
    fragColour[1] = vec4(1);
}
