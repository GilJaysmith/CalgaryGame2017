#version 150 core

in vec2 position;
in vec2 texcoord;

out vec2 Texcoord;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
  Texcoord = texcoord;
}
