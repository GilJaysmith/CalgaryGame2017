#version 150 core

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D tex;
uniform vec4 tint;

void main()
{
//	outColor = vec4(1.0, 0.0, 0.0, 1.0);
	outColor = texture(tex, Texcoord) * tint;
}
