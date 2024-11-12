#version 330 core

uniform sampler2D diffuseTex;  
uniform sampler2D icicleMask;  
uniform vec3 cameraPos;   

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    //vec3 tangent;
    //vec3 binormal;
    //vec3 worldPos;
} IN;

out vec4[2] fragColour; 

void main(void) 
{
    vec3 viewDir = normalize(cameraPos - IN.normal); 
    float cosTheta = dot(IN.normal, viewDir); 
    float fresnel = pow(1.0 - cosTheta, 5.0);  

    vec4 diffuse = texture(diffuseTex, IN.texCoord);  

    diffuse.rgb *= 1.5;
    vec4 rimColor = vec4(fresnel, fresnel, fresnel, 1.0); 

    fragColour[0] = diffuse; // + IN.colour;// +  (1-diffuse) * rimColor; 
    fragColour[1] = vec4(1);
}
