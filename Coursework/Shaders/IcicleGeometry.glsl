#version 330 core

uniform sampler2D icicleMask;  
uniform float iceHeight; 
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
} IN[];

out Vertex {
   vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec4 worldPos;
    float isFrozen;
} OUT;


void main() {
    for (int i = 0; i < gl_in.length(); i++) 
    {
        vec4 worldPos = modelMatrix * vec4(gl_in[i].gl_Position.xyz, 1.0);

        float frozen = 0;

        if (temperature < 0) {
            if (IN[i].normal.y < 0.0) {                    
            

                float mask = texture(icicleMask, IN[i].texCoord).r;
                if (mask > 0.45) {                
                    frozen = 1;

                    float maxBoundary = -10;
                    float minBoundary = 10;
                    float tempMult = clamp((maxBoundary - abs(temperature)) / (maxBoundary - minBoundary), 0.0, 1.0);
                    float displacement = iceHeight * mask * tempMult;
                    worldPos.y -= displacement;
                }
            }
        }

        OUT.tangent = vec3(1); // Meant for tess shader, but having issues there
        OUT.binormal = vec3(1);
        OUT.isFrozen = frozen;
        OUT.worldPos = worldPos;
        OUT.colour = IN[i].colour;
        OUT.texCoord = IN[i].texCoord;
        OUT.normal = IN[i].normal;

        gl_Position = projMatrix * viewMatrix * worldPos;
        EmitVertex();
    }
    EndPrimitive();
}
