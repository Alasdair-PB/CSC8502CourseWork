#version 400 core

layout(vertices=3) out;

uniform float tessLevelInner;
uniform float tessLevelOuter;

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
} OUT[];

patch out vec4 subColour;

void main(){
	gl_TessLevelInner[0] = tessLevelInner;
	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;

	OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;
    OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
    OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
    OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;
    OUT[gl_InvocationID].binormal = IN[gl_InvocationID].binormal;
    OUT[gl_InvocationID].worldPos = IN[gl_InvocationID].worldPos;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
