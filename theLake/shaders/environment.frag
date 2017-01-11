#version 430
 
out vec4 outputF;

in vec4 pos;
in vec4 n;
uniform float time;
uniform vec3 clear_color;

uniform vec3 lDir;
uniform vec2 window_dim;
uniform sampler2D skyTexture;


void main()
{

	vec2 screen_coord = vec2(gl_FragCoord.x / window_dim.x, -gl_FragCoord.y / window_dim.y );
	vec3 refraction_color = vec3(texture(skyTexture, screen_coord));


	vec3 start_color = vec3(0.7, 0.5, 0.4);
	vec3 end_color = vec3(0.2, 0.6, 0.8);

	vec3 color = clear_color + 0.0003 * abs(pos.y) * (end_color - 0.1 * start_color);
	outputF = vec4(refraction_color, 1.0);
	
 }