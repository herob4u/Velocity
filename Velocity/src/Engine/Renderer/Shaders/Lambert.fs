#version 330

uniform vec3 Color;

in vec3 normal_ES;
in vec3 pos_ES;

float Lambertian(vec3 pos, vec3 normal, vec3 lightPos)
{
	vec3 lightVec = normalize(lightPos - pos);
	return max(0, dot(normal, lightVec));
}

void main()
{
	gl_FragColor = vec4(Lambertian * Color, 1);
}