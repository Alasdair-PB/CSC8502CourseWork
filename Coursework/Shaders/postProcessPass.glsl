#version 330 core

uniform sampler2D diffuseTex;      
uniform sampler2D depthTex;   
uniform sampler2D fogTexture;   

uniform float fogDensity;  
uniform vec3 fogColor;     
uniform vec3 cameraPos;
uniform mat4 projMatrix;           
uniform mat4 inverseProjView;   
uniform float bendFactor = 1000; 

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

float getLinearDepth(float depthSample) 
{
    float near = 0.1;
    float far = 15000.0;
    return (2.0 * near) / (far + near - depthSample * (far - near));
}

vec3 reconstructWorldPosition(vec2 uv, float depth)
{
    vec4 ndcPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPos = inverseProjView * ndcPos;
    return worldPos.xyz / worldPos.w; 
}

void main(void)
{
    float depth = texture(depthTex, IN.texCoord).r;
    vec3 worldPos = reconstructWorldPosition(IN.texCoord, depth);

    float distance = length(cameraPos - worldPos);
    float linearDepth = getLinearDepth(depth);
    float fogFactor = exp(-pow(linearDepth * fogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec2 fogTexCoord = IN.texCoord + cameraPos.xz;

    vec4 fog = texture(fogTexture, fogTexCoord);
    fog.xyz *= fogColor;
    vec4 finalColor = vec4(diffuse.rgb, 1.0);
    finalColor.xyz = mix(fog.xyz, finalColor.xyz, fogFactor);

    if (diffuse.xyz == vec3(0, 1, 0))
        finalColor.a = 0;

    fragColour = finalColor;
}
