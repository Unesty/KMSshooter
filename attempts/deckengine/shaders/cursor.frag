#version 420
//precision mediump float;
out vec3 FragColor;
uniform vec2 iMouse;
void main()
{
	FragColor = vec3(gl_FragCoord.x/768*float((gl_FragCoord.x>(iMouse.x-20))&&(gl_FragCoord.x<(iMouse.x+20))&&(gl_FragCoord.y>(-iMouse.y-20))&&(gl_FragCoord.y<(-iMouse.y+20))),0.5,0.5);
}
 
