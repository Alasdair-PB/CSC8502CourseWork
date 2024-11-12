#version 400 core

layout(triangles, equal_spacing, cw) in;
uniform sampler2D icicleMask;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float iceHeight; 
uniform float temperature;

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

vec3 TriMixVec3(vec3 a, vec3 b, vec3 c) {
    vec3 p0 = mix(a, b, gl_TessCoord.x);
    vec3 p1 = mix(b, c, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

vec2 TriMixVec2(vec2 a, vec2 b, vec2 c) {
    vec2 p0 = mix(a, b, gl_TessCoord.x);
    vec2 p1 = mix(b, c, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

void main() {
    vec3 combinedPos = TriMixVec3(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
    OUT.texCoord = TriMixVec2(IN[0].texCoord, IN[1].texCoord, IN[2].texCoord);
    vec4 worldPos = modelMatrix * vec4(combinedPos, 1);

    vec3 edge1 = worldPos.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge2 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    OUT.normal = normalize(cross(edge1, edge2));

    gl_Position = projMatrix * viewMatrix * worldPos;

    vec2 deltaUV1 = IN[1].texCoord - IN[0].texCoord;
    vec2 deltaUV2 = IN[2].texCoord - IN[0].texCoord;

    vec3 deltaPos1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 deltaPos2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    vec3 tangent = r * (deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2);

    OUT.isFrozen = 0;
    OUT.tangent = normalize(tangent);
    OUT.binormal = normalize(cross(OUT.normal, OUT.tangent));
    OUT.colour = IN[0].colour;
    OUT.worldPos = worldPos;
}
