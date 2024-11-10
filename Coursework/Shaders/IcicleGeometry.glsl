#version 330 core

uniform sampler2D icicleMask;  
uniform float iceHeight; 
//uniform float temperature;

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

void main() {
    for (int i = 0; i < gl_in.length(); i++) 
    {
        vec4 pos = gl_in[i].gl_Position;
        vec3 normal = IN[i].normal;
        vec3 worldPos = IN[i].worldPos;
        OUT.iceShading = 0;

        if (normal.y < 0.0) {
            float mask = texture(icicleMask, IN[i].texCoord).r;

            if (mask > 0.45)
            {
                float displacement = iceHeight * mask;
                pos.y -= displacement;
                 OUT.iceShading = 1;
            } 
        }

        gl_Position = pos;

        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;
        OUT.worldPos = worldPos;
        
        EmitVertex();
    }
    EndPrimitive();
}
