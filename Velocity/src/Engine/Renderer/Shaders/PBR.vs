#version 330

#define MAX_SCENE_LIGHTS 16

in (layout=0) vec3 a_Position;
in (layout=1) vec3 a_Normal;
in (layout=2) vec2 a_TexCoord;
in (layout=3) vec3 a_Tangent;
in (layout=4) vec3 a_BiTangent;

out vec3 eye_Position;
out vec3 eye_Normal;
out vec2 TexCoord;
out mat3 TBN; // Tangent, BiTangent, Normal matrix to transform tangent space normals to world space
out mat3 InvNormalMatrix;

struct PointLight
{
	vec3 Position;
	vec3 Intensity;
};

struct DirectionalLight
{
	vec3 Direction;
	vec3 Intensity;
};

uniform SceneData
{
	mat4 View;
	mat4 Projection;
	mat4 NormalMatrix;

	DirectionalLight Sunlight;
	PointLight Lights[MAX_SCENE_LIGHTS];
} sceneData;

uniform mat4 Model;

void main()
{
	// Eye space quantities for fragment shader
	vec4 position_xyzw = vec4(a_Position, 1.f);
	eye_Position	= (sceneData.View * Model * position_xyzw).xyz;
	eye_Normal		= normalize(sceneData.NormalMatrix * a_Normal);

	// TBN Matrix for normal map sampling
	vec3 T = normalize(sceneData.NormalMatrix * a_Tangent);
	vec3 B = normalize(sceneData.NormalMatrix * a_BiTangent);
	TBN = mat3(T, B, eye_Normal);

	// Texture coordinates
	TexCoord = a_TexCoord;

	// Invert normal matrix here, less computations than doing it in fragment shader!
	InvNormalMatrix = transpose(sceneData.NormalMatrix);

	gl_Position = Projection * View * Model * vec4(a_Position, 1.0f);
}