#version 150 core

in vec3 Colour;
in vec2 Texcoord;

out vec4 outColour;

uniform sampler2D tex;
uniform float time;
uniform vec4 tint;

void main()
{
	// All white
	//outColour = vec4(1.0, 1.0, 1.0, 1.0);
	
	// Sampled from texture
	outColour = texture(tex, Texcoord) * Colour * tint;

	// Based on screen position of pixel
//	outColour = vec4(0.0, gl_FragCoord.y / 600.0, 0.0, 1.0);

	// Sampled from texture or wobbling reflection of texture
//	if (Texcoord.y < 0.5)
//		outColour = texture(tex, Texcoord);
//	else
//		outColour = texture(tex,
//			vec2(Texcoord.x + sin(Texcoord.y * 60.0 + time * 2.0) / 30.0, 1.0 - Texcoord.y)
//		) * vec4(0.7, 0.7, 1.0, 1.0);

	// Edges in white
	//if (Texcoord.x < 0.04 || Texcoord.x >= 0.96 || Texcoord.y < 0.04 || Texcoord.y >= 0.96)
	//	outColour = vec4(Colour, 1.0);
	//else
	//	outColour = vec4(0.0, 0.0, 0.0, 1.0);
}
