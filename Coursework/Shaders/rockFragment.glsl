#version 330 core

uniform sampler2D diffuseTex;  
uniform sampler2D rockBump;  

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
    vec4 diffuse = texture(diffuseTex, IN.texCoord);  
    vec3 normal = texture(rockBump,  IN.texCoord).rgb * 2.0 - 1;
    
    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    normal = normalize(TBN * normal);

    diffuse.a= 1;
    fragColour[0] = diffuse; 
    fragColour[1] = vec4(normal * 0.5 + 0.5, 1.0);
}
