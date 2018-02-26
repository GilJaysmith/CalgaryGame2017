#version 150 core

in vec3 position;
in vec3 colour;

out vec3 Colour;

uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
  Colour = colour;
  gl_Position = camera_projection * camera_view * vec4(position, 1.0);
}
