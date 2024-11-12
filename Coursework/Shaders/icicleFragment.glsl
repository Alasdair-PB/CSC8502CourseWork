#version 330 core

uniform sampler2D diffuseTex;  
uniform sampler2D iceTex;  

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
    vec4 diffuse;

    if (IN.isFrozen > 0){
        //diffuse = texture(iceTex, IN.texCoord);  
        diffuse.xyz = vec3(0,0,1); 
    } else{
        diffuse = texture(diffuseTex, IN.texCoord);  
    }
       
    diffuse.a = 1;
    fragColour[0] = diffuse; 
    fragColour[1] = vec4(1);
}
