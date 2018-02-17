#version 150 core

in vec3 position;
in vec3 normal;
in vec3 colour;

out vec3 Colour;
out vec3 FragPos;
out vec3 FragNormal;

uniform mat4 model_transform;
uniform mat4 model_normal_transform;
uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
	Colour = colour;
    FragPos = vec3(model_transform * vec4(position, 1.0));
    FragNormal = mat3(model_normal_transform) * normal;
    gl_Position = camera_projection * camera_view * vec4(FragPos, 1.0);
}
