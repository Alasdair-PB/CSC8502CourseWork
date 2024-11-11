#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;
const float scaleFactors[7] = float[](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);


void main(void) {

    vec3 light = texture(diffuseLight, IN.texCoord).xyz;
    vec3 specular = vec3(0.0);
    vec2 delta = vec2(0, 0);

	delta = dFdx(IN.texCoord) + dFdy(IN.texCoord);

    vec3 diffuse = vec3(0.0);

    for (int i = 0; i < 1; i++) {
        vec2 offset = vec2(0); //vec2(delta.x * (i - 3), delta.y * (i - 3));
        specular += texture(specularLight, IN.texCoord + offset).xyz;// * scaleFactors[i];
        diffuse += texture(diffuseTex, IN.texCoord + offset).xyz;// * scaleFactors[i];
    }

    fragColour.xyz = diffuse * 0.1; 
    fragColour.xyz += diffuse * light; 
    fragColour.xyz += specular;
    fragColour.a = (fragColour.r + fragColour.g + fragColour.b <= 0.075) ? 0.0 : 1.0;
}
