#version 420
uniform vec2 iMouse;
uniform float iTime;
in flat int id;
layout (early_fragment_tests) in;
out vec3 FragColor;

void main()
{
	FragColor = vec3(gl_PointCoord.x+gl_PointCoord.y,sin(iTime)/float(id),float(iMouse.x-10<gl_FragCoord.x&&iMouse.x+10>gl_FragCoord.x&&-iMouse.y+10>gl_FragCoord.y&&-iMouse.y-10<gl_FragCoord.y));
	//FragColor = vec3(1.0);
}
