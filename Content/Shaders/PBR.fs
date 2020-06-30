#version 400

out vec4 fragColor;

#define MAX_SCENE_LIGHTS 16

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
	mat4 NormalMatrix;

	DirectionalLight Sunlight;
	PointLight Lights[MAX_SCENE_LIGHTS];
} sceneData;

struct Maps
{
	sampler2D diffuse;
	sampler2D normal;
	sampler2D mask1; // x = metallicness, y = roughness, w = rim light
	samplerCube environment;
	sampler2D irradiance;
	sampler2D emissive;
};

uniform Maps maps;

uniform Material
{
	vec3 diffuse;
	vec4 mask1; // x = metallicness, y = roughness, w = rim light
	vec3 emissive;
} material;

out DATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN; // Tangent, BiTangent, Normal matrix to transform tangent space normals to world space
	mat3 InvNormalMatrix; 
} fs_in;

const float PI = 3.141592653589793;
const float lambertian = (1.0 / PI);
const float GAMMA = 2.2;

float saturate(in float val)
{
	return clamp(val, 0.f, 1.f);
}

vec3 GammaCorrect(vec3 color)
{
	return pow(color, vec3(1.0 / GAMMA));
}
// ------------------------------------ DIFFUSE MODELS ----------------------------------------- //

// Returns the diffuse factor based on Disney's method. Uses a fresnel factor to create
// a retroreflective finish as opposed to the dark shadows from Lambert Diffuse.
float Disney_Diffuse(float NdL, float NdV, float NdH, float roughness)
{
	// A bias factor for a fresnel effect that will boost output response for rougher surfaces
	// at grazing angles while attenuating smoother surfaces (ranges from 0.5 to 2.5). This is 
	// what gives rough materials their brighter, flatter color.
	float fd90 = 0.5 + 2 * (NdH * NdH) * roughness;
	float f0 = 1;

	// What is this energy factor? Not found in the Disney formulation
	float energyFactor = mix(1.0, 1.0 / 1.51, roughness);

	float light_fresnel = fresnel_schlick(f0, fd90, NdL);
	float view_fresnel  = fresnel_schlick(f0, fd90, NdV);

	return light_fresnel * view_fresnel;
}

float Lambert_Diffuse(float NdL)
{
	return (NdL * lambertian);
}

float OrenNayer_Diffuse()
{
	return 0;
}

// ------------------------------------ SPECULAR MODELS ----------------------------------------- //

// Sample from Distribution function for Cook-Torrance Specular.
// Note for GGX. Because of the alpha term in the numerator, if roughness is exactly 0, the specular highlight
// collapses and completely disappears (f(x) = 0), make sure you clamp it to small value.
float GGX(float NdH, float roughness)
{
	// Sample the GGX distribution for the D term that describes the specular lobe.
	float a = max(roughness * roughness, 0.005);
	float aa = a * a;

	float den = ((NdH * NdH) * (aa - 1) + 1);
	float D   = (aa) / (PI * den * den);
	
	return D;
}

// Fresnel Factor for Cook-Torrance Specular. float specular = f(diffuse, metallicness);
float Fresnel_UE4(float LdH, float specular)
{
	// The Unreal Engine fresnel factor approximation 
	LdH = saturate(LdH);
	float f0 = specular;
	float pwr = (-5.55473 * LdH - 6.98316) * LdH;
	return f0 + (1 - f0) * exp2(pwr);
}

// Geometric Factor for Cook-Torrance Specular
float G_Smith(float NdV, float NdL, float roughness)
{
	// Base on remaped roughness from 0.5 - 1 for better linear scaling shadowing effect
	float a = pow((0.5 + roughness/2), 2);

	float G_light = NdL / (NdL * (1 - a) + a);
	float G_view  = NdV / (NdV * (1 - a) + a);

	// Why the 0.25? Not found in Disney/UE4 paper
	return 0.25 / (G_light * G_view);
}

float Cook_Torrance_Specular(NdL, NdV, NdH, LdH, roughness)
{
	float NdL;
	float NdV;
	float NdH;
	float LdH;

	float roughness;
	float specular; // from metallicness
	// D * G * F / cosine term
	float D = GGX(NdH, roughness);
	float F = Fresnel_UE4(LdH, specular);
	float G = G_Smith(NdV, NdL, roughness);

	return (D * F * G) / (4 * NdL * NdV);
}

float Phong_Specular()
{

}


// -------------------- Material Params --------------------------/
vec3 GetDiffuse()
{
#ifdef HAS_DIFFUSE_MAP
	vec3 diffuse = texture(maps.diffuse, TexCoord).xyz;
#else
	vec3 diffuse = material.diffuse;
#endif
	return diffuse;
}

vec3 GetNormal()
{
	vec3 N = normalize(fs_in.normal);
	// Normal map
#ifdef HAS_NORMAL_MAP
	vec3 N = TBN * (2.0 * texture(maps.normal, fs_in.uv).xyz - 1.0);
#endif
	return N;
}

vec3 GetMask1()
{
#ifdef HAS_MASK1_MAP
	vec4 mask = texture(maps.mask1);

	float metallicness	= mask.r;
	float roughness		= mask.g;
#else
	float metallicness	= material.mask1.r;
	float roughness		= material.mask1.g;
#endif
	return vec3(metallicness, roughness, 0.0);
}

// ---------------- Redirecting Functions to Specific Models --------------//
void Diffuse(float NdL, float NdV, float NdH, float LdH, Material material)
{
	float roughness = material.mask1.y;
	return Disney_Diffuse(NdL, NdV, NdH, roughness);
}

void Specular(float NdL, float NdV, float NdH, float LdH, Material material)
{
	float roughness = material.mask1.y;
	return Cook_Torrance_Specular(NdL, NdV, NdH, LdH, roughness);
}

vec3 IBLIrradiance(float NdV, float roughness, vec3 specular)
{
	vec2 preintegratedFG = texture(maps.irradiance, vec2(roughness, 1.0 - NdV)).rg;
	return specular * preintegratedFG.r + preintegratedFG.g;
}

vec3 IBL(vec3 eye, Material material)
{
	float normal = normalize(fs_in.normal);
	float NdV = max(dot(normal, eye), 0.0);
	float roughness = material.mask1.y;
	float specular  = mix(vec3(0.04), material.diffuse, material.mask.x);

	vec3 reflectionVector = normalize(reflect(-eye, normal));
	float smoothness = 1.0 - roughness;
	float mipLevel = (1.0 - (smoothness * smoothness)) * 10.0;

	// Ambient term - essentially a precomputed FG
	vec4 cs = textureLod(maps.environment, reflectionVector, mipLevel);
	vec3 result = pow(cs.xyz, vec3(GAMMA)) * IBLIrradiance(NdV, roughness, specular);

	// The actual environment map sample
	vec3 diffuseDominantDirection = normal;
	float diffuseLowMip = 9.6;
	vec3 diffuseImageLighting = textureLod(maps.environment, diffuseDominantDirection, diffuseLowMip).rgb;
	diffuseImageLighting = pow(diffuseImageLighting, vec3(GAMMA));

	return result + diffuseImageLighting * material.albedo.rgb;
}


void main()
{
	vec3 V = normalize(-fs_in.position.xyz);
	vec3 N = normalize(GetNormal());

	// TBN transform

	// -------------- Material Parameters -----------------
	Material mat;
	mat.diffuse	= GetDiffuse();
	mat.mask1	= GetMask1();
	float metallic	= mat.mask1.x;
	float roughness = mat.mask1.y;
	
	// Metallic materials have specular highlights of similar color to the surface because they readily absorb incident light
	// @TODO: Can we do what SMITE does for metallicness? Directly sampled colors from a texture.
	vec3 specular = mix(vec3(0.04), diffuse, metallicness);

	// -------------- IBL -----------------
	// Sample the environment map based on the "look at" direction of the surface normal
	vec3 env_diffuse	= textureLod(maps.environment, InvNormalMatrix * N, 10).rgb;
	vec3 refl			= InvNormalMatrix * reflect(-V, N); // The reflection the eye sees from the environment
	vec3 env_spec		= textureLod(maps.environment, refl, max(roughness * 11.0, textureQueryLod(maps.environment, refl).y)).rgb;

	// -------------- Diffuse and Specular Factors --------------
	vec3 total_diffuse(0.0);
	vec3 total_specular(0.0);

	// -------------- Specular Model Computation -----------------
	// The following are essential quantities that captures the interaction of light with the surface
	// @TODO: Now only handling sunlight, this later changes into a for-loop that adds contributions from all scene lights
	for(int i = 0; i < MAX_SCENE_LIGHTS; i++)
	{
		vec3 L = normalize(sceneData.Lights[i].Position - V);
		vec3 H = normalize(L + V);

		float NdL = saturate(max(0.0, dot(N, L)));
		float NdV = max(0.001, dot(N, V));
		float NdH = max(0.001, dot(N, H));
		float LdH = max(0.001, dot(L, H)); // LdH == VdH

		// Remember, diffuse = kd * fd. specular = ks * fs.
		total_diffuse += Diffuse(NdL, NdV, NdH, LdH, mat) * sceneData.Lights[i].Intensity;
		total_specular += Specular(NdL, NdV, NdH, LdH, mat) * sceneData.Lights[i].Intensity;
	}

	// Shadow code here...
	float visibility = 1.0;
	float bias = 0.005;
	for(int i = 0; i < MAX_SCENE_LIGHTS; i++)
	{
		// Randomly sample shadow map using poisson disc
	}

	vec3 finalColor = mat.diffuse * total_diffuse * visibility + (total_specular) * visibility;
	finalColor = GammaCorrect(finalColor);
	fragColor = vec4(finalColor, 1.0);
}
