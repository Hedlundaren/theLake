#version 430
 
out vec4 outputF;

in vec3 newPos;
in vec4 n;
uniform float time;

uniform vec3 lDir;
uniform vec3 camPos;

void main()
{

	float depth =  0.001 * gl_FragCoord.z / gl_FragCoord.w;
	
	outputF = vec4(vec3(depth), 1.0);
	
 }