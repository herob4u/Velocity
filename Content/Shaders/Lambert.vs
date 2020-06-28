#version 330

// MeshVertex
in layout(location=0) vec3 a_Position;
in layout(location=1) vec3 a_Normal;
in layout(location=2) vec2 a_UV;
in layout(location=3) vec3 a_Tangent;
in layout(location=4) vec3 a_BiTangent;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;

out VS_out
{
	vec3 fragNormal;
	vec3 fragPos;
} vs_out;

void main()
{
	mat3 normalMatrix	= mat3(transpose(inverse(Model)));
	vec4 position		= vec4(a_Position, 1.f);

	vs_out.fragPos		= (View * Model * position).xyz;
	vs_out.fragNormal	= normalMatrix * normalize(a_Normal);

	gl_Position = Projection * View * Model * position;
}