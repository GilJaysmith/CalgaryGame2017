#version 150 core

in vec2 position;
in vec2 texcoord;
in vec4 colour;

out vec2 Texcoord;
out vec4 Colour;

void main()
{
	Texcoord = texcoord;
    Colour = colour;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
