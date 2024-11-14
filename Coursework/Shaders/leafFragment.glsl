#version 330 core

uniform sampler2D diffuseTex;  
uniform float temperature;
//uniform vec3 cameraPos;   

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4[2] fragColour; 

void main(void) 
{
    vec4 diffuse = texture(diffuseTex, IN.texCoord).rgba;  
    if(diffuse.a == 0.0) 
		discard;

    fragColour[0] = diffuse; 
    fragColour[1] = vec4(1);
}
