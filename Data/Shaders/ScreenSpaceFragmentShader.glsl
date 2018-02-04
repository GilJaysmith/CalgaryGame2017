#version 150 core

in vec2 Texcoord;
out vec4 outColour;

uniform sampler2D tex;
uniform vec4 tint;

void main()
{
//	outColour = vec4(1.0, 0.0, 0.0, 1.0);
	outColour = texture(tex, Texcoord) * tint;
}
