#version 420
layout (points) in;
layout (triangle_strip,max_vertices=6) out;
uniform vec4 deck[52];
void main()
{
	gl_Position = vec4(0.5,sin(iTime/8.0),.0,1.0);
	EmitVertex();
	gl_Position = vec4(-0.5,sin(iTime/8.0),.0,1.0);
	EmitVertex();
	gl_Position = vec4(0.5,-sin(iTime/8.0),.0,1.0);
	EmitVertex();
	gl_Position = vec4(-0.5,-sin(iTime/8.0),.0,1.0);
	EmitVertex();
	EndPrimitive();
}
