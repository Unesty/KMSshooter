#version 100
//precision mediump float;
//out vec3 FragColor;
uniform vec2 mouse;
void main()
{
	gl_FragColor = vec3(gl_FragCoord.x/768*float((gl_FragCoord.x>(mouse.x-20))&&(gl_FragCoord.x<(mouse.x+20))),gl_FragCoord.y/1366*float((gl_FragCoord.y>(-mouse.y-20))&&(gl_FragCoord.y<(-mouse.y+20))),0.5);
}
 
