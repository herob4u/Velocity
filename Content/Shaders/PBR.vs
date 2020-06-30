#version 330

#define MAX_SCENE_LIGHTS 16

in layout(location=0) vec3 a_Position;
in layout(location=1) vec3 a_Normal;
in layout(location=2) vec2 a_TexCoord;
in layout(location=3) vec3 a_Tangent;
in layout(location=4) vec3 a_BiTangent;

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

layout (std140) uniform SceneData
{
	mat4 View;
	mat4 Projection;
	mat3 NormalMatrix;

	DirectionalLight Sunlight;
	PointLight Lights[MAX_SCENE_LIGHTS];
} sceneData;

uniform mat4 Model;

out DATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN; // Tangent, BiTangent, Normal matrix to transform tangent space normals to world space
	mat3 InvNormalMatrix; 
} vs_out;

void main()
{
	// Eye space quantities for fragment shader
	vec4 position = vec4(a_Position, 1.f);

	vs_out.position	= (Model * position);
	vs_out.normal	= normalize(sceneData.NormalMatrix * a_Normal);

	// TBN Matrix for normal map sampling
	vec3 T = normalize(sceneData.NormalMatrix * a_Tangent);
	vec3 B = normalize(sceneData.NormalMatrix * a_BiTangent);
	vs_out.TBN = mat3(T, B, vs_out.normal);

	// Texture coordinates
	vs_out.uv = a_TexCoord;

	// Invert normal matrix here, less computations than doing it in fragment shader!
	sceneData.InvNormalMatrix = transpose(sceneData.NormalMatrix);

	gl_Position = sceneData.Projection * sceneData.View * Model * position;
}