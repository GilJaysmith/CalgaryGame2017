#version 150 core

in vec2 Texcoord;

out vec4 outColour;

uniform sampler2D tex;
uniform vec4 tint;

const float offset = 1.0 / 300.0;

void main()
{
  // Fading in/out
  //vec3 texColour = vec3(texture(tex, Texcoord));
  //vec3 diff = texColour - vec3(tint);
	//outColour = vec4(vec3(tint) + tint.w * diff, 1.0f);
  
  vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2( 0.0f, offset), // top-center
    vec2( offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2( 0.0f, 0.0f), // center-center
    vec2( offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f, -offset), // bottom-center
    vec2( offset, -offset) // bottom-right
  );

  // sharpen
  float kernel_sharpen[9] = float[](
    -1, -1, -1,
    -1, 9, -1,
    -1, -1, -1
  );
  
  // blur
  float kernel_blur[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
  );

  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
    sampleTex[i] = vec3(texture(tex, Texcoord.st + offsets[i]));
  }
  vec3 col = vec3(0.0);
  for(int i = 0; i < 9; i++)
  col += sampleTex[i] * kernel_sharpen[i];

  outColour = vec4(col, 1.0);
}
