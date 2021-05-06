#version 420

uniform ivec4 deck[52];
//uniform float iTime;
//uniform vec2 iMouse;
out flat int id;

void main()
{
	//gl_Position = vec4(deck.x+sin(iTime)+iMouse.x,deck.y+cos(iTime)-iMouse.y,deck.z,1.0);
	//gl_Position = vec4(float((gl_VertexID << 1) & 2)/2.-0.4, float(gl_VertexID & 2)/sin(iTime/8.), 0.0f, 1.0f);
	//gl_Position = vec4(float((gl_VertexID == 0)*),,);
	switch(gl_VertexID){
	case 0:
		gl_Position = vec4(4.0,4.0,1.0,1.0);
		id=0;
		return;break;
	case 1:
		gl_Position = vec4(-4.0,4.0,1.0,1.0);
		id=0;
		return;break;
	case 2:
		gl_Position = vec4(-4.0,-4.0,1.0,1.0);
		id=0;
		return;break;
	case 3:
		gl_Position = vec4(-4.0,-4.0,1.0,1.0);
		id=0;
		return;break;
	case 4:
		gl_Position = vec4(float(deck[51].x)/683.-0.9,float(deck[51].y)/384.-0.8,0.,1.);
		id=51;
		return;break;
	case 5:
		gl_Position = vec4(float(deck[51].x)/683.-0.9,float(deck[51].y)/384.-0.8,0.,1.);
		id=51;
		return;break;
	case 6:
		gl_Position = vec4(deck[51].xy/vec2(683,384)+vec2(0.1,-0.2)-1.0,0.,1.);
		id=51;
		return;break;
	case 7:
		gl_Position = vec4(deck[51].xy/vec2(683,384)+vec2(-0.1,0.2)-1.0,0.,1.);
		id=51;
		return;break;
	case 8:
		gl_Position = vec4(deck[51].xy/vec2(683,384)+vec2(-0.1,-0.2)-1.0,0.,1.);
		id=51;
		return;break;
	case 9:
		gl_Position = vec4(deck[51].xy/vec2(683,384)+vec2(-0.1,-0.2)-1.0,0.,1.);
		id=51;
		return;break;
	case 10:
		gl_Position = vec4(deck[1].xy/vec2(683,384)+vec2(0.1,0.2)-1.0,0.5,1.);
		id=1;
		return;break;
	case 11:
		gl_Position = vec4(deck[1].xy/vec2(683,384)+vec2(0.1,0.2)-1.0,0.5,1.);
		id=1;
		return;break;
	case 12:
		gl_Position = vec4(deck[1].xy/vec2(683,384)+vec2(0.1,-0.2)-1.0,0.5,1.);
		id=1;
		return;break;
	case 13:
		gl_Position = vec4(deck[1].xy/vec2(683,384)+vec2(-0.1,0.2)-1.0,0.5,1.);
		id=1;
		return;break;
	case 14:
		gl_Position = vec4(deck[1].xy/vec2(683,384)+vec2(-0.1,-0.2)-1.0,0.5,1.);
		id=1;
		return;break;
	}
}
