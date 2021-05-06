#version 460
precision mediump int;

out vec3 FragColor;
void main()
{
	FragColor = vec3(gl_FragCoord);
}
