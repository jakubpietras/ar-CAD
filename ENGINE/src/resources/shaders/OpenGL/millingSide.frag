#version 450 core
in vec3 WorldPosFrag;
in vec3 NormFrag;
uniform vec3 u_CameraPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
out vec4 FragColor;  

uniform vec3 u_SurfaceColor;

const float ka = 0.3f, kd = 0.8f, ks = 0.2f, m = 64.f;
const vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);

void main()
{
	vec3 normal = normalize(NormFrag);
	vec3 lightDir = normalize(u_LightPos - WorldPosFrag);

	// ambient
	vec3 ambient = u_SurfaceColor * ka;

	// diffuse
	float NdotL = dot(normal, lightDir);
	float diff = max(NdotL, 0.0f);
	vec3 diffuse = kd * u_SurfaceColor * diff;

	// specular
	vec3 viewDir = normalize(u_CameraPos - WorldPosFrag);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), m);
	vec3 specular = (NdotL > 0.0) ? ks * spec * vec3(1.0f) : vec3(0.5f);


	vec3 result = diffuse;
	FragColor = vec4(result, 1.0f);
}