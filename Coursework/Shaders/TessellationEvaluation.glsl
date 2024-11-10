#version 400 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main()
{
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;

	vec3 combinedPos = p0 + p1 + p2;
	vec4 worldPos = modelMatrix * vec4(combinedPos, 1);

	OUT.colour = IN[0].colour; 
    OUT.texCoord = IN[0].texCoord;
    OUT.normal = normalize(IN[0].normal); 
    OUT.tangent = normalize(IN[0].tangent);
    OUT.binormal = normalize(IN[0].binormal);
    OUT.worldPos = IN[0].worldPos.xyz;

	gl_Position = projMatrix * viewMatrix * worldPos;
}
