#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;
const float scaleFactors[7] = float[](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);

void main(void) 
{
    vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;


	/*fragColour = vec4(0, 0, 0, 1);
	vec2 delta = vec2(0, 0);

	delta = dFdy(IN.texCoord);
	
	for (int i = 0; i < 7; i++) 
	{
		vec2 offset = delta * (i - 3);
		vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + offset);
		fragColour += tmp * scaleFactors[i];
	}*/

	fragColour.xyz = diffuse;
	fragColour.a = 1;
	fragColour.a = (fragColour.r + fragColour.g + fragColour.b <= 0.075) ? 0.0 : 1.0;
}
