#version 330 core

uniform sampler2D diffuseTex;  
uniform sampler2D iceTex;  
uniform sampler2D iceBumpTex;  
uniform sampler2D woodTex;  
uniform sampler2D woodBumpTex;  


in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec4 worldPos;
    float isFrozen;
} IN;

out vec4 fragColour[2]; 

void main(void)
{
     mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

     vec4 diffuse = texture(iceTex, IN.texCoord);  
     vec3 normal = texture(iceBumpTex, IN.texCoord).rgb * 2.0 - 1.0;

     vec4 iceDiffuse = texture(diffuseTex, IN.texCoord);  
     vec3 iceNormal = texture(woodBumpTex, IN.texCoord).rgb * 2.0 - 1.0;

     diffuse = mix(iceDiffuse, diffuse, IN.isFrozen);
     normal = mix(iceNormal, normal, IN.isFrozen);

     normal = normalize(TBN * normal);

     diffuse.a = 1;
     fragColour[0] = diffuse; 
     fragColour[1] = vec4(normal * 0.5 + 0.5, 1.0);
}
