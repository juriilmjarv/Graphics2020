#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tc;
layout (location = 2) in vec3 normals;

out VS_OUT
{
    vec2 tc;
    vec4 normals;
	vec4 fragPos;
	vec3 fPosition;
} vs_out;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position, 1.0);
	vs_out.fPosition = position;
    vs_out.tc = tc;
    //vs_out.normals = normals;
	vec3 normalsT = mat3(transpose(inverse(model_matrix))) * normals.xyz;
	vs_out.normals = vec4(normalsT, 1.0);
	vs_out.fragPos = model_matrix * vec4(position, 1.0);
}
