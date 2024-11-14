#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D iceTex;
uniform sampler2D iceTexBump;

uniform sampler2D depthTex; 
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform float temperature;
uniform float scrollSpeed;
uniform float dt;
uniform float transparency;
uniform float waterScale;

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
	vec3 tangent;
	vec3 binormal;
	vec4 worldPos;
} IN;

out vec4[2] fragColour; 

void main(void) 
{
    float waterCycle = dt * scrollSpeed; 
    vec2 scrolledTexCoord = IN.texCoord + vec2(waterCycle, waterCycle);
    vec2 scaledTexCoord = scrolledTexCoord * waterScale;

    vec4 diffuse = texture(diffuseTex, scaledTexCoord); 
    vec3 viewDir = normalize(cameraPos - IN.worldPos.xyz); 

    vec3 reflectDir = reflect(-viewDir, normalize(IN.normal)); 
    vec4 reflectTex = texture(cubeTex, reflectDir); 

    vec2 uv = gl_FragCoord.xy / vec2(dimensions.x,dimensions.y);
    float sceneDepth = texture(depthTex, uv).r;

    vec3 sceneWorldPos = depthToWorldPos(uv, sceneDepth);

    float fragmentDepth = gl_FragCoord.z;
    vec3 fragmentWorldPos = depthToWorldPos(uv, fragmentDepth);

    float depthDiff = length(fragmentWorldPos - sceneWorldPos);
    
    vec3 normal = IN.normal;
    if (depthDiff >= 5.0 * clamp(40 - temperature, 1, 100)) 
    {  
        fragColour[0] = mix(reflectTex, diffuse, 0.5);
        mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
        normal = texture(bumpTex, scaledTexCoord).rgb * 2.0 - 1;
        normal = normalize(TBN * normal);
    } 
    else 
    {
        vec4 diffuse = texture(iceTex,  IN.texCoord * waterScale); 
        fragColour[0] = mix(reflectTex, diffuse, 0.25);

        mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
        normal = texture(iceTexBump,  IN.texCoord * waterScale).rgb * 2.0 - 1;
        normal = normalize(TBN * normal);
    }

    fragColour[0].w = transparency;
    fragColour[1] = vec4(IN.normal * 0.5 + 0.5, 1.0);
}
