#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D depthTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) 
{
    float depth = texture(depthTex, IN.texCoord).r;
    vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
    vec3 specular = texture(specularLight, IN.texCoord).xyz;
    vec3 light = texture(diffuseLight, IN.texCoord).xyz;

    fragColour.xyz = diffuse;// * 0.1; 
    //fragColour.xyz += diffuse * light;
    fragColour.xyz += specular;

    // Green screen
    if (depth == 1)
        fragColour.xyz = vec3(0,1,0);
    fragColour.a = 1.0;
}
