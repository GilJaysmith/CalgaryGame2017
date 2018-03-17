#version 150 core

in vec3 position;

uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
  gl_Position = camera_projection * camera_view * vec4(position, 1.0);
}
