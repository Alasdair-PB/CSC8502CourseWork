#version 400 core

layout (quads, equal_spacing, cw) in;

uniform sampler2D depthTex;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float dt;
uniform float frequency;
uniform float temperature;


in Vertex{
	vec4 colour;
	vec2 texCoord;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec4 worldPos;
} OUT;

vec3 QuadMixVec3(vec3 a, vec3 b, vec3 c, vec3 d) {
    vec3 p0 = mix(a, c, gl_TessCoord.x);
    vec3 p1 = mix(b, d, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

vec2 QuadMixVec2 ( vec2 a , vec2 b , vec2 c , vec2 d ) {
    vec2 p0 = mix (a ,c , gl_TessCoord.x);
    vec2 p1 = mix (b ,d , gl_TessCoord.x);
    return mix (p0 , p1 , gl_TessCoord.y);
}

void main () 
{
    vec3 combinedPos = QuadMixVec3(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_in[3].gl_Position.xyz);
    OUT.texCoord = QuadMixVec2(IN[0].texCoord, IN[1].texCoord, IN[2].texCoord, IN[3].texCoord);

    vec4 worldPos = modelMatrix * vec4(combinedPos, 1);

    float frequency = 0.2;
    worldPos.y += 10.0 * sin(-worldPos.x * frequency + -worldPos.z * frequency + dt);

    gl_Position = projMatrix * viewMatrix * worldPos;

    vec3 edge1 = worldPos.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge2 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    OUT.normal = normalize(cross(edge1, edge2));

    vec2 deltaUV1 = IN[1].texCoord - IN[0].texCoord;
    vec2 deltaUV2 = IN[2].texCoord - IN[0].texCoord;
    vec3 deltaPos1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 deltaPos2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    vec3 tangent = r * (deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2);

    OUT.tangent = normalize(tangent);
    OUT.binormal = normalize(cross(OUT.normal, OUT.tangent));
    OUT.colour = IN[0].colour; 
    OUT.worldPos = worldPos;
}
