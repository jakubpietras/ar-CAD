#version 450 core

in vec3 WorldPosFrag;
in vec3 NormFrag;
uniform vec3 u_CameraPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

out vec4 FragColor;  

const float kd = 0.5f, ks = 0.2f, m = 100.f;
const vec3 ambientColor = {0.2f, 0.2f, 0.2f};
const vec3 surfaceColor = {1.f, 1.f, 1.f};

void main()
{
	vec3 viewVec = normalize(u_CameraPos - WorldPosFrag);
	vec3 normal = normalize(NormFrag);
	vec3 color = surfaceColor * ambientColor;

	vec3 lightVec = normalize(u_LightPos - WorldPosFrag);
	vec3 halfVec = normalize(viewVec + lightVec);
	color += u_LightColor * surfaceColor * kd * clamp(dot(normal, lightVec), 0.0, 1.0);
	float nh = dot(normal, halfVec);
	nh = clamp(nh, 0.0, 1.0);
	nh = pow(nh, m);
	nh *= ks;
	color += u_LightColor * nh;

	FragColor = vec4(
	clamp(color.r, 0.0f, 1.0f),
	clamp(color.g, 0.0f, 1.0f),
	clamp(color.b, 0.0f, 1.0f),
	1.0f);
}