#version 150 core

in vec3 Texcoord;

out vec4 outColour;

uniform samplerCube tex;

void main()
{
	outColour = texture(tex, Texcoord);
}
