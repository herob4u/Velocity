#version 330

in vec3 local_position;

uniform samplerCube environmentMap;

out vec4 fragColor;

void main()
{
	vec3 color = texture(environmentMap, local_position).rgb;

	// Color correct for conversion from HDR to LDR
	color = color / (color + vec3(1.0));

	// Gamma correction for HDRs in linear range
    color = pow(color, vec3(1.0/2.2)); 

	fragColor = vec4(color, 1.0);
}