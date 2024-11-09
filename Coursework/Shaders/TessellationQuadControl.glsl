#version 400 core

layout(vertices=3) out;

uniform float tessLevelInner;
uniform float tessLevelOuter;

in Vertex{
	vec4 colour;
} IN[];

out Vertex {
	vec4 colour;
} OUT[];

patch out vec4 subColour;

void main(){
	gl_TessLevelInner[0] = tessLevelInner;
	gl_TessLevelInner[1] = tessLevelInner; 

	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;
	gl_TessLevelOuter[3] = tessLevelOuter; 

	//barrier ();
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	//OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;

}