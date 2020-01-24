#version 150 core

in vec3 position;
in vec3 colour;
in vec2 texcoord;

out vec3 Colour;
out vec2 Texcoord;

uniform mat4 model_transform;
uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
	Colour = colour;
	Texcoord = vec2(texcoord.x, texcoord.y);
	gl_Position = camera_projection * camera_view * model_transform * vec4(position, 1.0);
}
