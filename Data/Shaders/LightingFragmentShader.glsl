#version 150 core

in vec3 Normal;
in vec3 Colour;
in vec3 FragPos;

out vec4 outColour;

uniform vec3 ambient_colour;
uniform vec3 diffuse_position;
uniform vec3 diffuse_colour;
uniform vec3 view_position;

void main()
{
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * ambient_colour;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(diffuse_position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuse_colour;
    
    float specular_strength = 1.0;
    float specular_power = 16.0;
    vec3 viewDir = normalize(view_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular_power);
    vec3 specular = specular_strength * spec * diffuse_colour;
    
    vec3 final_colour = (ambient + diffuse + specular) * Colour;
	outColour = vec4(final_colour, 1.0);
}
