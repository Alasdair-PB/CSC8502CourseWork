#version 400 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float dt; // Pass time from the application for motion

vec3 QuadMixVec3(vec3 a, vec3 b, vec3 c, vec3 d) 
{
    vec3 p0 = mix(a, c, gl_TessCoord.x);
    vec3 p1 = mix(b, d, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

void main () {
    // Interpolate position within the quad
    vec3 combinedPos = QuadMixVec3(
        gl_in[0].gl_Position.xyz,
        gl_in[1].gl_Position.xyz,
        gl_in[2].gl_Position.xyz,
        gl_in[3].gl_Position.xyz
    );

    // Add sine wave motion along the Y-axis (or another axis if desired)
    float waveAmplitude = 0.1; // Adjust amplitude as needed
    float waveFrequency = 2.0; // Adjust frequency as needed
    combinedPos.y += waveAmplitude * sin(waveFrequency * combinedPos.x + dt);

    // Transform to world and then to clip space
    vec4 worldPos = modelMatrix * vec4(combinedPos, 1);
    gl_Position = projMatrix * viewMatrix * worldPos;
}
