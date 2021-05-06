#version 420
//layout (points) in;
in struct {
	vec4 pos;
	uint id;
} v;
layout (triangle_strip,max_vertices=52) out;
uniform vec4 deck[52];
void main()
{
	for(uint n=id+1;n<51;n++){
		if(deck[id].xy+ivec2(1366/5,768/10)/5>deck[n].xy&&deck[id].xy-ivec2(1366/5,768/10)<deck[n].xy){
			gl_Position = vec4(deck[n].xy/vec2(1366.,768.),0.0,1.0);
			EmitVertex();
			gl_Position = vec4(deck[n].xy/vec2(1366.,768.)+vec2(-0.2,0.1),0.0,1.0);
			EmitVertex();
			gl_Position = vec4(deck[n].xy/vec2(1366.,768.)+vec2(0.2,-0.1),0.0,1.0);
			EmitVertex();
		}
	}
	
	gl_Position = vec4(0.2,0.1,.0,1.0);
	EmitVertex();
	gl_Position = vec4(-0.2,0.1,.0,1.0);
	EmitVertex();
	gl_Position = vec4(0.2,-0.1,.0,1.0);
	EmitVertex();
	gl_Position = vec4(-0.2,-0.1,.0,1.0);
	EmitVertex();
	EndPrimitive();
}
