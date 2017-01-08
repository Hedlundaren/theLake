#version 430

out vec4 outputF;


void main()
{
	vec3 red = vec3(1.0, 0.0, 0.0);
	outputF = vec4(red, 1.0);
} 