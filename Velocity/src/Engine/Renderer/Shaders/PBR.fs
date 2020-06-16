#version 330

in vec3 eye_Position;
in vec3 eye_Normal;
in vec2 TexCoord;
in mat3 TBN;
in mat3 InvNormalMatrix;

uniform Maps
{
	sampler2D diffuse;
	sampler2D normal;
	sampler2D mask1; // x = metallicness, y = roughness, w = rim light
	samplerCube environment;
	sampler2D irradiance;
} maps;

uniform Material
{
	vec4 diffuse;
	vec4 mask1; // x = metallicness, y = roughness, w = rim light
} material;

const float pi = 3.141592653589793;
const float lambertian = (1.0 / pi);

float saturate(in float val)
{
	return clamp(val, 0.f, 1.f);
}

// Schlick's Fresnel factor approximation where f0 is the reflectance at angle of incidence and cos_theta is the cosine of the incident angle
// For non-metals, f0 = 0.04
vec3 fresnel_factor(in vec3 f0, in float cos_theta)
{
	// Schlick’s Approximation. See: https://graphicscompendium.com/raytracing/11-fresnel-beer
	// This returns the probability of reflection
    return mix(f0, vec3(1.0), pow(1.01 - cos_theta, 5.0));
}

// UE4 implementation for computing cook-torrance specular lighting terms
// These are the distribution functions D. Given a direction, the value of D indicates
// the percentage of microfacets facing that direction.
// The RMS microfacet slope, m, is assumed to be roughness^2 by Disney. This was carried on
// by the UE4 implementation.

float D_blinn(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
}

float D_beckmann(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
}

float D_GGX(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
}

float G_schlick(in float roughness, in float NdV, in float NdL)
{
	// the UE4 implementation remaps roughness for analytic light sources as follows
	// roughness = (roughness + 1)/2, which leads to k = (roughness + 1)^2 / 8
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}


// Supported Specular Models
// Cook-Torrance Specular Model
vec3 CT_specular(in float NdL, in float NdV, in float NdH, in vec3 fresnel_factor, in float roughness)
{

}

vec3 Sunlight(vec3 V, vec3 N, vec3 sunDir, vec3 sunIntensity)
{
	vec3 L = -sunDir;
	vec3 H = normalize(L+V);


}

void main()
{
	vec3 V = normalize(-eye_Position);
	vec3 N = normalize(eye_Normal);

	//vec3 output = Sunlight(V, N, sceneData.Sunlight.Direction, sceneData.Sunlight.Intensity);

	// TBN transform

	// -------------- Material Parameters -----------------
	// Diffuse Map
#ifdef HAS_DIFFUSE_MAP
	vec3 diffuse = texture(maps.diffuse, TexCoord).xyz;
#else
	vec3 diffuse = material.diffuse.rgb;
#endif

	// Normal map
#ifdef HAS_NORMAL_MAP
	N = TBN * (2.0 * texture(maps.normal, TexCoord).xyz - 1.0);
#endif

	// Metallicness & Roughness
#ifdef HAS_MASK1_MAP
	vec4 mask = texture(maps.mask1);

	float metallicness	= mask.r;
	float roughness		= mask.g;
#else
	float metallicness	= material.mask1.r;
	float roughness		= material.mask1.g;
#endif
	
	// Metallic materials have specular highlights of similar color to the surface because they readily absorb incident light
	// @TODO: Can we do what SMITE does for metallicness? Directly sampled colors from a texture.
	vec3 specular = mix(vec3(0.04), diffuse, metallicness);

	// -------------- IBL -----------------
	// Sample the environment map based on the "look at" direction of the surface normal
	vec3 env_diffuse	= textureLod(maps.environment, InvNormalMatrix * N, 10).rgb;
	vec3 refl			= InvNormalMatrix * reflect(-V, N); // The reflection the eye sees from the environment
	vec3 env_spec		= textureLod(maps.environment, refl, max(roughness * 11.0, textureQueryLod(maps.environment, refl).y)).rgb;

	// -------------- Specular Model Computation -----------------
	// The following are essential quantities that captures the interaction of light with the surface
	// @TODO: Now only handling sunlight, this later changes into a for-loop that adds contributions from all scene lights
	vec3 L = -sceneData.Sunlight.Direction; // We want a vector that points TOWARDS the light, hence the opposite of the light direction
	vec3 H = normalize(L + V);

	float NdL = max(0.0, dot(N, L));
	float NdV = max(0.001, dot(N, V));
	float NdH = max(0.001, dot(N, H));
	float HdV = max(0.001, dot(H, V));
	float LdV = max(0.001, dot(L, V));

	// Cook Torrance Method for specularity
	vec3 spec_fresnel = fresnel_factor(specular, HdV); // HdV == NdH, see page 3 of: https://inst.eecs.berkeley.edu/~cs283/sp13/lectures/cookpaper.pdf
	vec3 spec_reflectance = CT_specular(NdL, NdV, NdH, spec_fresnel, roughness);

	spec_reflectance *= vec3(NdL); // Visible specularity is weighed by the amount of light hitting the surface

	// Lambertian Diffuse - experiment with this by removing fresnel expression
	vec3 diffuse_reflectance = (vec3(1.0) - spec_fresnel) * NdL * lambertian;

	vec3 reflected_light	= vec3(0.f);
	vec3 diffuse_light		= vec3(0.f);


	// Notice for IBL, engine code must ensure to place at least a BLACK 1x1 texture for null entries
	vec2 ibl_diffuse = texture(maps.irradiance, vec2(roughness, 1 - NdV)).xy;
	vec3 ibl_spec = min(vec3(0.99), fresnel_factor(specular, NdV) * ibl_diffuse.x + ibl_diffuse.y);

	reflected_light += ibl_spec * env_spec;
	diffuse_light   += ibl_diffuse * env_diffuse;

	// Final fragment color
	// Linear combination of diffuse and specular models
	vec3 color = diffuse_light * mix(diffuse, vec3(0.0), metallicness) + reflected_light;
	gl_FragColor = vec4(color, 1.0);
}
