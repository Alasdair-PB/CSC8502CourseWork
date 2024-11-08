#version 330 core

uniform sampler2D diffuseTex;  
uniform vec3 cameraPosition;   

in Vertex{
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
} IN;

out vec4 fragColour;         

void main(void) 
{
    vec3 viewDir = normalize(cameraPosition - IN.normal); 
    float cosTheta = dot(IN.normal, viewDir); 
    float fresnel = pow(1.0 - cosTheta, 5.0);  

    vec4 diffuse = texture(diffuseTex, IN.texCoord);  
        vec4 rimColor = vec4(fresnel, fresnel, fresnel, 1.0); 

    fragColour = diffuse + rimColor; 
}
