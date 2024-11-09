#version 330 core

uniform float wiggleIntensity; 
uniform float wiggleFrequency;
uniform float dt;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in Vertex {
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
    vec3 lastWorldPos;
    for (int i = 0; i < gl_in.length(); i++) 
    {
        vec2 worldPos = vec2(i, i);
        vec4 pos = gl_in[i].gl_Position;

        float offsetX = sin(dt * wiggleFrequency + pos.x) * wiggleIntensity;
        float offsetY = cos(dt * wiggleFrequency + pos.y) * wiggleIntensity;

        gl_Position = pos + vec4(offsetX, offsetY, 0.0, 0.0);

        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;
        EmitVertex();
    }
    EndPrimitive();
}