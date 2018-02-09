#version 150 core

in vec2 Texcoord;
in vec4 Colour;

out vec4 outColour;

uniform sampler2D tex;

void main()
{
	outColour = texture(tex, Texcoord) * Colour;
}
