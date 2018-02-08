#version 150 core

in vec3 Normal;
in vec3 Colour;
in vec3 FragPos;

out vec4 outColour;

uniform vec3 lighting_ambient_colour;
uniform vec3 lighting_directional_vector;
uniform vec3 lighting_directional_colour;
uniform vec3 camera_position;

void main()
{
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * lighting_ambient_colour;
    
    vec3 lightDir = normalize(lighting_directional_vector - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lighting_directional_colour;
    
    float specular_strength = 1.0;
    float specular_power = 16.0;
    vec3 viewDir = normalize(camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular_power);
    vec3 specular = specular_strength * spec * lighting_directional_colour;
    
    vec3 final_colour = (ambient + diffuse + specular) * Colour;
	outColour = vec4(final_colour, 1.0);
}
