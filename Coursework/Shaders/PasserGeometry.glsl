#version 330 core

uniform float wiggleIntensity = 0.75; 
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



void main() {
    for (int i = 0; i < gl_in.length(); i++) 
    {
        vec4 pos = gl_in[i].gl_Position;

        float offsetX = 0;
        float offsetY = 0;

        gl_Position = pos + vec4(offsetX, offsetY, 0.0, 0.0);

        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;
		OUT.tangent = IN[i].tangent;
		OUT.binormal = IN[i].binormal;
		OUT.worldPos = IN[i].worldPos;
        EmitVertex();
    }
    EndPrimitive();
}
