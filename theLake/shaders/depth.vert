#version 430
 
in vec4 position;
in vec4 normal;
 
out vec3 newPos;
out vec4 color;
out vec4 n;

uniform float time;

uniform mat4 MV;
uniform mat4 P;

void main()
{
	newPos = position.xyz;
	n = ( normal );

    gl_Position = P * MV * position;
} 