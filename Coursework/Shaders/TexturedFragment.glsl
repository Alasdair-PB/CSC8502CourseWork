#version 330 core

uniform sampler2D diffuseTex;  
uniform vec3 cameraPos;   

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
    vec3 viewDir = normalize(cameraPos - IN.normal); 
    float cosTheta = dot(IN.normal, viewDir); 
    float fresnel = pow(1.0 - cosTheta, 5.0);  

    vec4 diffuse = texture(diffuseTex, IN.texCoord);  
    vec4 rimColor = vec4(fresnel, fresnel, fresnel, 1.0); 

    fragColour[0] = mix(diffuse, rimColor, 0.5f); 
    fragColour[1] = vec4(0.5,0.5,0.5,1);
}
