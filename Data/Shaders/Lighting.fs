#version 150 core

in vec3 Colour;
in vec3 FragPos;
in vec3 FragNormal;

out vec4 outColour;

uniform vec3 lighting_ambient_colour;
uniform vec3 lighting_directional_vector;
uniform vec3 lighting_directional_colour;
uniform vec3 camera_position;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lighting_ambient_colour;
  	
    // diffuse 
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(lighting_directional_vector);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lighting_directional_colour;
    
    // specular
    float specularStrength = 1.0;
	float specularPower = 32.0;
    vec3 viewDir = normalize(camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    vec3 specular = specularStrength * spec * lighting_directional_colour;  
        
    vec3 result = (ambient + diffuse + specular) * Colour;
    outColour = vec4(result, 1.0);
}
