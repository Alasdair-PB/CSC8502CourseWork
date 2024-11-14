#version 330 core

in vec2 texCoord;
in vec4 colour;

out vec4 fragColour; 

uniform sampler2D sprite;

void main()
{
    fragColour = (texture(sprite, texCoord) * colour);
} 