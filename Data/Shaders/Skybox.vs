#version 150 core

in vec3 position;

out vec3 Texcoord;

uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
	Texcoord = position;
    vec4 pos = camera_projection * camera_view * vec4(position, 1.0);
	gl_Position = pos.xyww;
}
