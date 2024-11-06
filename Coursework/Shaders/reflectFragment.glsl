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

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; 
    return (2.0 * projMatrix[3][2]) / (z - projMatrix[2][2]);
}

vec3 depthToWorldPos(vec2 uv, float depth) {
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, depth, 1.0); 
    vec4 viewSpacePos = inverse(projMatrix) * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;
    return (inverse(viewMatrix) * viewSpacePos).xyz; 
}

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

    vec2 uv = gl_FragCoord.xy / vec2(1920.0, 1080.0); 

    float sceneDepth = texture(depthTex, uv).r;

    float linearDepth = linearizeDepth(sceneDepth);
    vec3 sceneWorldPos = depthToWorldPos(uv, sceneDepth);

    float depthDiff = length(IN.worldPos - sceneWorldPos);
    float foamIntensity = clamp((depthDiff - foamCutoff) / foamCutoff, 0.0, 1.0);

    //fragColour = mix(reflectTex, vec4(1.0, 1.0, 1.0, transparency), foamIntensity);

     if (foamIntensity == 1f) 
     {
        fragColour = vec4(1.0, 0, 0, 1.0); 
    } else {
        fragColour = reflectTex + (diffuse * 0.25f);
        fragColour.w = transparency;

    }


}
