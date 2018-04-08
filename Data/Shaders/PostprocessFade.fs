#version 150 core

in vec2 Texcoord;

out vec4 outColour;

uniform sampler2D tex;
uniform vec4 tint;

void main()
{
  vec3 texColour = vec3(texture(tex, Texcoord));
  vec3 diff = texColour - vec3(tint);
	outColour = vec4(vec3(tint) + tint.w * diff, 1.0f);
}
