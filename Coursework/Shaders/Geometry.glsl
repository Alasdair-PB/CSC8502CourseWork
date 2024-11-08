#version 330 core

uniform float wiggleIntensity = 0.75; 
uniform float wiggleFrequency = 2.5;
uniform float dt;


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;

} IN[];

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;

} OUT;



void main() {
    for (int i = 0; i < gl_in.length(); i++) {

        float offsetX = sin(dt * wiggleFrequency + gl_in[i].gl_Position.y) * wiggleIntensity;
        float offsetY = cos(dt * wiggleFrequency + gl_in[i].gl_Position.x) * wiggleIntensity;

        gl_Position = gl_in[i].gl_Position + vec4(offsetX, offsetY, 0.0, 0.0);
        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;

        EmitVertex();
    }
    EndPrimitive();
}
