#version 420
precision mediump int;
//uniform mat4 modelviewMatrix;
uniform vec4 posoffvector;

attribute vec4 in_position;

void main()
{
	gl_Position = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) * (in_position+vec4(posoffvector.x,posoffvector.y,-3,1));
	//gl_Position = mat4(1,0,0,0,0,1,0,0,posoffvector.x,posoffvector.y,1,0,0,0,0,1) * in_position;
}  
