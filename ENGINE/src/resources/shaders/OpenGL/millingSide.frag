#version 450 core
in vec3 WorldPosFrag;
in vec3 NormFrag;
uniform vec3 u_CameraPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
out vec4 FragColor;  

const float kd = 0.5f, ks = 0.2f, m = 100.f;
const vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
const vec3 surfaceColor = vec3(1.f, 1.f, 1.f);

void main()
{
		vec3 normal = normalize(NormFrag);
	vec3 lightDir = normalize(u_LightPos - WorldPosFrag);

	// ambient
	vec3 ambient = ambientColor; // * texture()

	// diffuse
	float NdotL = dot(normal, lightDir);
	float diff = max(NdotL, 0.0f);
	vec3 diffuse = kd * surfaceColor * diff; // * texture()

	// specular
	vec3 viewDir = normalize(u_CameraPos - WorldPosFrag);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), m);
	vec3 specular = ks * spec * vec3(1.0f);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
}