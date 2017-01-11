#version 430
 
in vec4 position;
in vec4 normal;
 
out vec4 color;
out vec4 pos;
out vec4 n;

uniform float time;

uniform mat4 MV;
uniform mat4 P;

void main()
{
	pos = position;
	n = ( normal );

    gl_Position = P * MV * position;
} 