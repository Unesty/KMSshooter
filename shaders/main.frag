#version 100
precision mediump float;

varying vec4 vVaryingColor;

void main()
{
    gl_FragColor = vVaryingColor;
}
