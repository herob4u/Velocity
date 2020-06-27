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
	vec3 normal_ES;
	vec3 pos_ES;
} vs_out;

void main()
{
	vec4 position = vec4(a_Position, 1.f);

	// Excuse the misnomer
	mat4 ModelView = View * Model;

	vs_out.pos_ES = (ModelView * position).xyz;
	vs_out.normal_ES = normalize(mat3(ModelView) * a_Normal);

	gl_Position = Projection * View * Model * position;
}