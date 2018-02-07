#version 150 core

in vec3 position;
in vec3 normal;
in vec3 colour;

out vec3 Normal;
out vec3 Colour;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	Colour = colour;
    Normal = normal;
    FragPos = vec3(model * vec4(position, 1.0));
    gl_Position = proj * view * model * vec4(position, 1.0);
}
