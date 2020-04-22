#version 330

in (layout=0) vec3 a_Position;
in (layout=1) vec3 a_Normal;
in (layout=2) vec2 a_UV;

struct LightSource
{
	vec3 Pos;
	vec3 Intensity;
};
uniform LightSource Light;

uniform mat4 ViewProjection;
uniform mat4 Model;

out vec3 normal_ES;
out vec3 pos_ES;

void main()
{
	vec4 position = vec4(a_Position, 1.f);

	pos_ES = (ModelView * position).xyz;
	normal_ES = normalize(mat3(ModelView) * a_Normal); // Must be changed to NormalMatrix

	gl_Position = ViewProjection * Model * vec4(a_Position, 1.f);
}