#version 420
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
out vec3 fragColor;
// The MIT License
// Copyright © 2019 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


// EXACT distance to a pyramid. This shader computes the exact euclidean
// distances (not a bound based on half spaces). This allows to do
// operations on the shape such as rounding (see http://iquilezles.org/www/articles/distfunctions/distfunctions.htm)
// while other implementations don't. Unfortunately the maths require us to do
// one square root sometimes to get the exact distance.

// Other distande functions (SDFs):
//
// Quad:          https://www.shadertoy.com/view/Md2BWW
// Triangle:      https://www.shadertoy.com/view/4sXXRN
// Rounded Cone:  https://www.shadertoy.com/view/tdXGWr
// Cylinder:      https://www.shadertoy.com/view/wdXGDr
// Cone:          https://www.shadertoy.com/view/tsSXzK
// Octahedron:    https://www.shadertoy.com/view/wsSGDG
// Capped Torus:  https://www.shadertoy.com/view/tl23RK
// Solid Angle:   https://www.shadertoy.com/view/wtjSDW
// Pyramid:       https://www.shadertoy.com/view/Ws3SDl
// Many more:     https://www.shadertoy.com/view/Xds3zN
//
// List of primitive SDFs at http://iquilezles.org/www/articles/distfunctions/distfunctions.htm


// signed distance to a pyramid of base 1x1 and height h
float sdPyramid( in vec3 p, in float h )
{
    float m2 = h*h + 0.25;
    
    // symmetry
    p.xz = abs(p.xz); // do p=abs(p) instead for double pyramid
    p.xz = (p.z>p.x) ? p.zx : p.xz;
    p.xz -= 0.5;
	
    // project into face plane (2D)
    vec3 q = vec3( p.z, h*p.y - 0.5*p.x, h*p.x + 0.5*p.y);
   
    float s = max(-q.x,0.0);
    float t = clamp( (q.y-0.5*q.x)/(m2+0.25), 0.0, 1.0 );
    
    float a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
	float b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);
    
    float d2 = min(q.y,-q.x*m2-q.y*0.5) > 0.0 ? 0.0 : min(a,b);
    
    // recover 3D and scale, and add sign
    return sqrt( (d2+q.z*q.z)/m2 ) * sign(max(q.z,-p.y));;
}


float map( in vec3 pos )
{
    float rad = 0.2*(0.5-0.45*cos(iTime*3.0));
    float hei = 1.5*(0.5+0.45*sin(iTime*1.0));
    return sdPyramid(pos,hei) - rad;
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773;
    const float eps = 0.001;
    return normalize( e.xyy*map( pos + e.xyy*eps ) + 
					  e.yyx*map( pos + e.yyx*eps ) + 
					  e.yxy*map( pos + e.yxy*eps ) + 
					  e.xxx*map( pos + e.xxx*eps ) );
}
    
#define AA 2

void main()
{
     // camera movement	
	float an = 0.5*iTime;
	vec3 ro = vec3( 3.0*cos(an), 1.0, 3.0*sin(an) );
    vec3 ta = vec3( 0.0, 0.5, 0.0 );
    // camera matrix
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));

    
    
    vec3 tot = vec3(0.0);
    
    #if AA>1
    for( int m=0; m<AA; m++ )
    for( int n=0; n<AA; n++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
        vec2 p = (-iResolution.xy + 2.0*(gl_FragCoord.xy+o))/iResolution.y;
        #else    
        vec2 p = (-iResolution.xy + 2.0*gl_FragCoord.xy)/iResolution.y;
        #endif

	    // create view ray
        vec3 rd = normalize( p.x*uu + p.y*vv + 1.8*ww );

        // raymarch
        const float tmax = 30.0;
        float t = 0.0;
        for( int i=0; i<1024; i++ )
        {
            vec3 pos = ro + t*rd;
            float h = map(pos);
            if( h<0.0001 || t>tmax ) break;
            t += h;
        }
        
    
        // shading/lighting	
        vec3 col = vec3(0.0);
        if( t<tmax )
        {
            vec3 pos = ro + t*rd;
            vec3 nor = calcNormal(pos);
            float dif = clamp( dot(nor,vec3(0.7,0.6,0.4)), 0.0, 1.0 );
            float amb = 0.5 + 0.5*dot(nor,vec3(0.0,0.8,0.6));
            col = vec3(0.2,0.3,0.4)*amb + vec3(0.8,0.7,0.5)*dif;
        }

        // gamma        
        col = sqrt( col );
	    tot += col;
    #if AA>1
    }
    tot /= float(AA*AA);
    #endif

	fragColor = tot;
}
