#version 330 core
uniform samplerCube cubeTex;
uniform float temperature;

in Vertex {
	vec3 viewDir;
} IN ;

out vec4 fragColour;

void main ( void ) {
	fragColour = texture(cubeTex , normalize (IN.viewDir));
	fragColour.w = abs(clamp((temperature + 40) * 0.05f, 0, 1));
}