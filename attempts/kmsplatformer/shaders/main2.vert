#version 460
precision mediump int;
struct UBO {
 vec2 posoffvector;
};

layout (location=0) in vec3 in_position;
layout (location=0) out vec3 out_position;

void main()
{
	out_position = in_position+vec3(UBO.posoffvector,0);
}
