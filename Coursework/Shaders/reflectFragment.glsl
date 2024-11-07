#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D depthTex; 
uniform samplerCube cubeTex;

uniform vec3 cameraPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform float transparency;
uniform float foamCutoff;
uniform float foamSpeed;
uniform float uFarPlane;
uniform float dt;


uniform vec2 dimensions;

vec3 depthToWorldPos(vec2 uv, float depth) 
{ 
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); 
    vec4 viewSpacePos = inverse(projMatrix) * clipSpacePos; 
    viewSpacePos /= viewSpacePos.w; 
    return (inverse(viewMatrix) * viewSpacePos).xyz;
}

in Vertex 
{
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



    vec2 uv = gl_FragCoord.xy / vec2(dimensions.x,dimensions.y);
    float sceneDepth = texture(depthTex, uv).r;

    vec3 sceneWorldPos = depthToWorldPos(uv, sceneDepth);

    float fragmentDepth = gl_FragCoord.z;
    vec3 fragmentWorldPos = depthToWorldPos(uv, fragmentDepth);

    float depthDiff = length(fragmentWorldPos - sceneWorldPos);


    
    if (depthDiff >= 5.0) {  
        fragColour = (reflectTex * 0.5) + (diffuse * 0.5);
    } else {
        fragColour = vec4(1.0, 1.0, 1.0, 1.0); 
    }

    fragColour.w = transparency;
}
