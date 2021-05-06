precision mediump int;
uniform vec4 posoffvector;

attribute vec4 in_position;

void main()
{
	gl_Position = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) * (in_position+vec4(posoffvector.x,posoffvector.y,-3,1));
}  
