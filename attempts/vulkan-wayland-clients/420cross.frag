#version 420
precision mediump float;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform UBO {
	float Time; // in [0, 1], wrapping around
	vec2 Mouse;
} i;
void main()
{
	FragColor = vec4(vec3((gl_FragCoord.x+0.3)/768*float((gl_FragCoord.x>(i.Mouse.x+sin(i.Time)*5.-20))&&(gl_FragCoord.x<(i.Mouse.x+sin(i.Time)*5.+20))),gl_FragCoord.y/1366*float((gl_FragCoord.y>(-i.Mouse.y-20))&&(gl_FragCoord.y<(-i.Mouse.y+20))),0.5),0.5);
}
 
