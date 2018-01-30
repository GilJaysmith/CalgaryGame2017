#version 150 core

in vec3 position;
in vec3 colour;

out vec3 Colour;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	Colour = colour;
    gl_Position = proj * view * vec4(position, 1.0);
}
