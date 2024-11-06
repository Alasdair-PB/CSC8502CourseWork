#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform float transparency;
uniform float refractionRatio; 

uniform float uFarPlane;


uniform float depthDistance;
uniform float dt;


in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) 
{
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    
    vec3 reflectDir = reflect(-viewDir, normalize(IN.normal));
    vec4 reflectTex = texture(cubeTex, reflectDir);
    
    float sceneDepth = gl_FragCoord.z / gl_FragCoord.w;
    float linearDepth = sceneDepth * uFarPlane;

    float saturatedValue = clamp((linearDepth - gl_FragCoord.w) / depthDistance, 0.0, 1.0);



    fragColour = reflectTex + (diffuse * 0.25f);
    fragColour.w = transparency; 
}
