#version 420
precision mediump float;
out vec3 FragColor;
uniform vec2 iMouse;
uniform float iTime;
void main()
{
	FragColor = vec3((gl_FragCoord.x+0.3)/768*float((gl_FragCoord.x>(iMouse.x*sin(iTime)-20))&&(gl_FragCoord.x<(iMouse.x*sin(iTime)+20))),gl_FragCoord.y/1366*float((gl_FragCoord.y>(-iMouse.y-20))&&(gl_FragCoord.y<(-iMouse.y+20))),0.5);
}
 
