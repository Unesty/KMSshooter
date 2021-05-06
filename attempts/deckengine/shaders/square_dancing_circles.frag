#version 420
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
out vec3 fragColor;
float circle(in vec2 _st,in float _radius){
  vec2 l=_st-vec2(.5);
  return 1.-smoothstep(_radius-(_radius*.01),
  _radius+(_radius*.01),
  dot(l,l)*4.);
}

vec2 moveAndTile(vec2 st,float _zoom){
  st*=_zoom;
  
  // slow down time
  float time=iTime/2.;
  
  // naming some variables for ease of reading
  float stepCounter=step(.5,fract(time));
  float colTracker=step(1.,mod(st.y,2.));
  float rowTracker=step(1.,mod(st.x,2.));
  
  // when stepCounter is 1 (which is half the time), we move side to side
  // alternating rows move alternating directions
  st.x-=2.*fract(time)*(1.-colTracker)*stepCounter;
  st.x+=2.*fract(time)*colTracker*stepCounter;
  
  // otherwise when stepCounter is 0, we move up and down
  // still alternating directions
  st.y+=2.*fract(time)*rowTracker*(1.-stepCounter);
  st.y-=2.*fract(time)*(1.-rowTracker)*(1.-stepCounter);
  
  return fract(st);
}

void main()
{
  vec2 myRes = vec2(iResolution.x, iResolution.y/0.5);
  vec2 st=gl_FragCoord.xy/myRes.xy;
  vec3 color=vec3(0.);
  st=moveAndTile(st,20.);
  
  color=vec3(circle(st,.5));
  
  fragColor=color;
}
