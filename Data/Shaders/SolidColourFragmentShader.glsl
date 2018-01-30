#version 150 core

in vec3 Colour;

out vec4 outColor;

void main()
{
	// Solid colour
	outColor = vec4(Colour, 1.0f);
}
