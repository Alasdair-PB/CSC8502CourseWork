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
    float iceShading;

} OUT;

float randomSeed(vec3 pos) {
    return fract(sin(dot(pos, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}


void main() 
{
    vec3 lastWorldPos;
    for (int i = 0; i < gl_in.length(); i++) 
    {
        float leafSeed = randomSeed(IN[i].worldPos.xyz);
        vec4 worldPos = vec4(IN[i].worldPos.xyz,1);

        float offsetX = sin(dt * wiggleFrequency + worldPos.x) * wiggleIntensity;
        float offsetY = cos(dt * wiggleFrequency + worldPos.y) * wiggleIntensity;
         worldPos += vec4(offsetX, offsetY, 0.0, 0.0);        
        
        if (temperature < 0)
        {
            worldPos.y -= 50 * abs(IN[i].normal.x) * dtSeason;
            worldPos.x += 25 * IN[i].normal.y * dtSeason;
        } else if (dtSeason < 10)
        {
        // Can still use dt to work out if this is second time -> scale then offset
            float scaleFactor = dtSeason; 
            worldPos = vec4(worldPos.xyz * scaleFactor, 1);
        }

        gl_Position = projMatrix * viewMatrix *  worldPos;

        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;
        OUT.iceShading = 0;
        EmitVertex();
    }
    EndPrimitive();
}
