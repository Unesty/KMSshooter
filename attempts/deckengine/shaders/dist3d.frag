#version 420
#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718

out vec3 FragColor;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;


// Reference to
// http://thndl.com/square-shaped-shaders.html

void main(){
  vec2 st = vec2(gl_FragCoord.x/iResolution.x,gl_FragCoord.y/iResolution.y);
  //vec2 st = (gl_FragCoord.xy*2.0 - iResolution) / min(iResolution.x,iResolution.y);
//  st.x *= iResolution.x/iResolution.y;
 vec3 color = vec3(0.0);
 // float d = 0.0;

  // Remap the space to -1. to 1.
//  st = st *2.-1.;

  // Number of sides of your shape
//  int N = 3;

  // Angle and radius from the current pixel
 // float a = atan(st.x,st.y)+PI;
 // float r = TWO_PI/float(N);

  // Shaping function that modulate the distance
 // d = cos(floor(.5+a/r)*r-a)*length(st);

  //color = vec3(1.0-smoothstep(.4,.41,d));
  //color = vec3(d);
    mat4 frustum=mat4(
     1.0,0.0,0.0,0.0,
     0.0,1.0,0.0,0.000,
     0.0,0.0,-(10.000+0.01)/(10.0-0.01),-2.0*10.0*0.01/(10.0-0.01),
     0.0,0.000,-1.0,0.0);
float ox=iMouse.y/1000.0;
mat4 rotx=mat4(
    1.0,0.0,0.0,0.0,
	0.0,cos(ox),-sin(ox),0.0,
	0.0,sin(ox),cos(ox),0.0,
	0.0,0.0,0.0,1.0);
float oy=iMouse.x/1000.0;
mat4 roty=mat4(
    cos(oy),0.0,sin(oy),0.0,
    0.0,1.0,0.0,0.0,
    -sin(oy),0.0,cos(oy),0.0,
    0.0,0.0,0.0,1.0
);
float oz=iMouse.x/1000.;
mat4 rotz=mat4(cos(oz),-sin(oz),0.0,0.0,
               sin(oz),cos(oz),0.0,0.0,
               0.0,0.0,1.0,0.0,
               0.0,0.0,0.0,1.0
);
  vec4 ve = vec4(rotz*rotx*roty*(vec4((st.x-0.9),(st.y-0.3),1.0,1.0)));
  //color = vec3(ve.x>0.18&&ve.x<0.2320&&ve.z<0.0562&&ve.z>-0.0606)+vec3((ve.x+sin(iTime)/5.)*ve.z>0.18&&(ve.x+sin(iTime)/5.)*ve.z<0.2320&&(ve.y+sin(iTime)/5.)*ve.z<0.0552&&(ve.x+sin(iTime)/5.)*ve.z>-0.0596)+vec3(ve.x>0.08&&ve.x<0.1320&&ve.z<0.0552&&ve.z>-0.0596) + ve.xyz/4.0;
  //color = vec3(80. - (ve.x-sin(iTime/10.0))*(ve.x-sin(iTime/10.0)) - (ve.y-cos(iTime/10.))*(ve.y-cos(iTime/10.0)));
  
  
  color = abs(1./ve.zzz); 

  FragColor = color;
}
