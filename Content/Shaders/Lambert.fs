#version 330

struct LightSource
{
	vec3 position;
	vec3 intensity;
};
uniform LightSource light;

in VS_out
{
	vec3 fragNormal;
	vec3 fragPos;
} fs_in;

struct Material
{
	vec4 color;
	vec3 ambient;
};

uniform Material material;

void main()
{
	vec3 ambient = normalize(vec3(69, 111, 124));

	vec3 norm		= normalize(fs_in.fragNormal);
	vec3 pos		= fs_in.fragPos;
	vec3 lightDir	= normalize(light.position - pos);

	float lambert		= max(dot(norm, lightDir), 0.0);
	vec3 illumination	= (lambert * light.intensity) + ambient;

	if(light.position.y > 2)
		gl_FragColor = vec4(norm, 1.f);
	else
		gl_FragColor = vec4(illumination * material.color.xyz, 1.f);
}