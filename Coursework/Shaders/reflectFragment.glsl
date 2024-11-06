#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform float transparency;
uniform float refractionRatio; 

uniform float uFarPlane;

uniform float depthDistance;
uniform float dt;

uniform float foamCutoff;
uniform float foamSpeed;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float gradientNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    float result = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
    return result;
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

    float worldSpaceDepth = length(IN.worldPos - cameraPos);
    float saturatedValue = clamp((worldSpaceDepth - depthDistance) / depthDistance, 0.0, 1.0);

    vec2 uv = gl_FragCoord.xy / vec2(1920.0, 1080.0); 
    float noiseValue = gradientNoise(uv + dt * foamSpeed);  
    float foamEffect = smoothstep(foamCutoff - 0.05, foamCutoff + 0.05, saturatedValue - noiseValue);

    if (foamEffect > 0.0) {
        fragColour = vec4(1.0, 1.0, 1.0, transparency); 
    } else {
        fragColour = mix(reflectTex, diffuse * 0.25, foamEffect);
    }

    fragColour.w = transparency;
}
