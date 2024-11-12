#version 400 core

layout(vertices=3) out;

uniform float tessLevelInner = 10;
uniform float tessLevelOuter = 10;

in Vertex {
    vec4 colour;
    vec2 texCoord;
} IN[];

out Vertex {
    vec4 colour;
    vec2 texCoord;
} OUT[];

patch out vec4 subColour;

void main() {
    gl_TessLevelInner[0] = tessLevelInner;

    gl_TessLevelOuter[0] = tessLevelOuter;
    gl_TessLevelOuter[1] = tessLevelOuter;
    gl_TessLevelOuter[2] = tessLevelOuter;

    OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;
    OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}