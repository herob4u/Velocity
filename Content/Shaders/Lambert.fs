#version 330

uniform vec3 Color;

struct LightSource
{
	vec3 position;
	vec3 intensity;
};
uniform LightSource light;

in VS_out
{
	vec3 normal_ES;
	vec3 pos_ES;
} fs_in;

struct Material
{
	vec4 color;
	vec3 ambient;
};

uniform Material material;

float Lambertian(vec3 pos, vec3 normal, vec3 lightPos)
{
	vec3 lightVec = normalize(lightPos - pos);
	return max(0, dot(normal, lightVec));
}

void main()
{
	vec3 ambient = normalize(vec3(69, 111, 124));

	float lambert = Lambertian(fs_in.pos_ES, fs_in.normal_ES, light.position);
	vec3 illumination = lambert * (light.intensity + ambient);

	gl_FragColor = vec4(illumination * material.color.xyz, 1.f);
}