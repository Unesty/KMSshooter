#version 420
uniform ivec4 deck[52];
void main(){
	gl_Position=vec4(float(deck[gl_VertexID].xy)/vec2(1366.,768.),0.0,1.0);
}
