#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D shadowTex;

uniform sampler2D snowTex;
uniform sampler2D snowBumpTex;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;

in Vertex {
    vec3 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
    vec4 shadowProj; 
} IN;

out vec4 fragColour[2]; 

void main(void) 
{

    vec4 diffuse;
    vec3 normal;

    vec3 worldUp = vec3(0.0, 1.0, 0.0);
        float dotProduct = dot(normalize(IN.normal), worldUp);
    
    bool snowCovered = abs(dotProduct) > 0.9 && IN.worldPos.y > 50; 
    
    if (snowCovered)
    {        
        diffuse = texture(snowTex,  IN.texCoord); 
        normal = texture(snowBumpTex,  IN.texCoord).rgb * 2.0 - 1;
    }
    else
    {
        diffuse = texture(diffuseTex,  IN.texCoord); 
        normal = texture(bumpTex,  IN.texCoord).rgb * 2.0 - 1;
    }

    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    normal = normalize(TBN * normal);

    float shadow = 1.0; 
    vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
    
    if (abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
        vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
        float shadowZ = texture(shadowTex, biasCoord.xy).x;
        
        if (shadowZ < biasCoord.z) 
            shadow = 0.4f;
    }

    fragColour[0].rgb = diffuse.rgb;
    fragColour[0].rgb *= shadow; 
    fragColour[0].a = diffuse.a; 
    fragColour[1] = vec4(normal * 0.5 + 0.5, 1.0);
}
