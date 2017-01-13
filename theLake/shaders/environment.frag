#version 430
 
out vec4 outputF;

in vec3 newPos;
in vec3 newNormal;
in vec2 texCoord;

uniform float time;
uniform vec3 clear_color;

uniform vec3 lDir;
uniform vec2 window_dim;
uniform sampler2D skyTexture;


void main()
{

	//vec2 screen_coord = vec2(gl_FragCoord.x / window_dim.x, -gl_FragCoord.y / window_dim.y );
	//vec3 refraction_color = vec3(texture(skyTexture, screen_coord));


	//vec3 start_color = vec3(0.7, 0.5, 0.4);
	//vec3 end_color = vec3(0.2, 0.6, 0.8);

	vec3 earth = vec3(texture(skyTexture, texCoord));

	outputF = vec4(earth, 1.0);
	
 }