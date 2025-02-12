#version 330 core

uniform float wiggleIntensity; 
uniform float wiggleFrequency;
uniform float dtSeason;
uniform float dt;

uniform float temperature;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

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

float randomSeed(vec3 pos) {
    return fract(sin(dot(pos, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}


void main() 
{
    vec3 leafCenter = (IN[0].worldPos + IN[1].worldPos + IN[2].worldPos) / 3.0;

    for (int i = 0; i < gl_in.length(); i++) 
    {
        float leafSeed = randomSeed(IN[i].worldPos.xyz);
        vec4 worldPos = vec4(IN[i].worldPos.xyz,1);

        float offsetX = sin(dt * wiggleFrequency + worldPos.x) * wiggleIntensity;
        float offsetY = cos(dt * wiggleFrequency + worldPos.y) * wiggleIntensity;

        worldPos += vec4(offsetX, offsetY, 0.0, 0.0);        
        gl_Position = projMatrix * viewMatrix *  worldPos;


        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;
        EmitVertex();
    }
    EndPrimitive();
}
