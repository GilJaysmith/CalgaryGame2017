#version 150 core

in vec3 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

void main()
{
	Color = color;
	Texcoord = vec2(texcoord.x, texcoord.y);
	gl_Position = proj * view * model * vec4(position.x * (0.5 + sin(time) / 2.0), position.y * (0.5 + sin(time) / 2.0), position.z * (0.5 + sin(time) / 2.0), 1.0);
}
