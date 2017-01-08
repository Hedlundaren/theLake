#version 430

in vec3 normal;
in vec3 in_color;
in vec3 pos;
in vec3 sphere_center;

out vec4 outputF;

uniform float time;
uniform vec3 camPos;
uniform vec3 lDir;

uniform float testData;

void main()
{

	float kd = 0.2;
	float diffuse = kd * (max(dot(lDir, normal), 0.0));
	vec3 diffuse_color = vec3(diffuse);
	float depth = pow( normalize(gl_FragCoord.z / gl_FragCoord.w), 6.0) - 0.9;
	vec3 water_color = vec3(diffuse + depth);
	water_color.b += 0.4;
	water_color.g += 0.0;
	outputF = vec4(water_color, 1.0);
} 