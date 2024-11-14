#version 330 core

uniform sampler2D diffuseTex;      
uniform sampler2D depthTex;   

uniform float fogDensity;   
uniform vec3 fogColor;     
uniform vec3 cameraPos;
uniform mat4 projMatrix;           
uniform mat4 inverseProjView;          

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
    float depth = texture(depthTex, IN.texCoord).r;
    vec3 ndcPos = vec3(IN.texCoord, depth) * 2.0 - 1.0;

    vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
    vec3 worldPos = invClipPos.xyz / invClipPos.w;
    float distance = length(cameraPos - worldPos);



    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    vec4 finalColor = vec4(diffuse.rgb,1);

    if (distance > 4000){
        finalColor.xyz *= fogColor;
    }

    fragColour = finalColor;


    if (diffuse.xyz == vec3(0,1,0))
		fragColour.a = 0;


}
