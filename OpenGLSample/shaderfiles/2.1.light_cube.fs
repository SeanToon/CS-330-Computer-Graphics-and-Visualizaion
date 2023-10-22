#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal; //normals
in vec2 TexCoords;

struct Light {
	vec3 color;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

void main()
{
    vec3 ambient = light.ambient * light.color; 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * light.color; 

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); 
    vec3 specular = light.specular * spec * light.color; 

    vec3 result = (ambient + diffuse + specular); 

    FragColor = vec4(result, 1.0);
}


