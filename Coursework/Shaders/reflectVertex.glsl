#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float dt;
uniform float scrollSpeed;
uniform float waterScale;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main(void) 
{
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

    float waterCycle = dt * scrollSpeed; 
    vec2 scrolledTexCoord = texCoord + vec2(waterCycle, waterCycle);

    vec2 scaledTexCoord = scrolledTexCoord * waterScale;
    OUT.texCoord = scaledTexCoord;
    OUT.normal = wNormal;

    vec4 worldPos = modelMatrix * vec4(position, 1);


	OUT.colour = colour;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal) * tangent.w;

	OUT.worldPos = worldPos.xyz;
	gl_Position = (projMatrix * viewMatrix) * worldPos;
}
