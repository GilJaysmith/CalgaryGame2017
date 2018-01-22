#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D tex;
uniform float time;

void main()
{
	if (Texcoord.y < 0.5)
		outColor = texture(tex, Texcoord);
	else
		outColor = texture(tex,
			vec2(Texcoord.x + sin(Texcoord.y * 60.0 + time * 2.0) / 30.0, 1.0 - Texcoord.y)
		) * vec4(0.7, 0.7, 1.0, 1.0);
	//if (Texcoord.x < 0.04 || Texcoord.x >= 0.96 || Texcoord.y < 0.04 || Texcoord.y >= 0.96)
	//	outColor = vec4(Color, 1.0);
	//else
	//	outColor = vec4(0.0, 0.0, 0.0, 1.0);
}
