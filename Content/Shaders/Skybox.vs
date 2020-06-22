#version 330

in layout(location=0) vec3 a_Position;
in layout(location=1) vec3 a_UVW;

uniform mat4 Projection;
uniform mat4 View;

out vec3 local_position;

void main()
{
	local_position = a_Position;

	mat4 rotMat = mat4(mat3(View));

	// Introduce the w-component in a such a way that the same position is yielded after perspective division
	vec4 clipPos = Projection * View * vec4(local_position, 1.0);

	// Now, by using z = w, will always be rendered at a distance of 1.0
	gl_Position = clipPos.xyww;
}