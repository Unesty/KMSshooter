#version 420
precision mediump float;
out vec3 FragColor;
uniform vec2 mouse;
void main()
{
	FragColor = vec3(768*float(((gl_FragCoord.x*gl_FragCoord.x)>(gl_FragCoord.y*gl_FragCoord.y-mouse.y)))*1366*float(((gl_FragCoord.y*gl_FragCoord.y)>(gl_FragCoord.x*gl_FragCoord.x-mouse.x))),0.5,0.5);
}
 
