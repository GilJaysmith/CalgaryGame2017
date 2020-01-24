#version 150 core

in vec3 position;

uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 world_transform;

void main()
{
  gl_Position = camera_projection * camera_view * world_transform * vec4(position, 1.0);
}
