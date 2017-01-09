#version 430
 
out vec4 outputF;

in vec4 pos;
in vec4 n;
uniform float time;

uniform vec3 lDir;

void main()
{

	float c = sin(time + pos.x + sin(pos.y*30.0 + pos.x*10 + time)) * 0.5 + 0.5;

	vec3 normal = normalize(cross(dFdx(pos.xyz), dFdy(pos.xyz)));


	float intensity = pow( 0.7 - dot( n.xyz, vec3( 0.0, 0.0, 0.5 ) ), 7.0 ); 

    outputF = vec4(c, 0.0, 0.0, 1.0);
	outputF = vec4( 1.0, 0.8, 0.5, 1.0 ) * intensity;
	outputF = n;
	
 }