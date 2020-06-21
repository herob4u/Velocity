#version 330

in vec2 texCoord;
uniform sampler2D diffuseTexture;

void main()
{
	vec4 diffuse = texture(diffuseTexture, texCoord);
	gl_FragColor = diffuse;
}