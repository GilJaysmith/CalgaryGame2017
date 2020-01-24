#version 150 core

uniform vec3 tint;

out vec4 outColour;

void main()
{
	outColour = vec4(tint, 1.0f);
}
