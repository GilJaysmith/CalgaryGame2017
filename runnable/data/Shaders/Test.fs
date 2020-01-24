#version 150 core

in vec3 Colour;
in vec2 Texcoord;

out vec4 outColour;

uniform sampler2D tex;
uniform vec4 model_tint;

void main()
{
	// Sampled from texture
	outColour = texture(tex, Texcoord) * vec4(Colour, 1.0) * model_tint;
}
