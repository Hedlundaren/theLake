#version 430


out vec4 outputF;

in vec3 newPos;

uniform float time;
uniform vec3 camPos;
uniform vec3 lDir;
uniform vec3 clear_color;
uniform vec2 window_dim;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D depthTexture;

vec3 mod289(vec3 x)
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
	return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
	return t*t*t*(t*(t*6.0-15.0)+10.0);
}

		

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
	vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
	vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period
	Pi0 = mod289(Pi0);
	Pi1 = mod289(Pi1);
	vec3 Pf0 = fract(P); // Fractional part for interpolation
	vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
	vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
	vec4 iy = vec4(Pi0.yy, Pi1.yy);
	vec4 iz0 = Pi0.zzzz;
	vec4 iz1 = Pi1.zzzz;

	vec4 ixy = permute(permute(ix) + iy);
	vec4 ixy0 = permute(ixy + iz0);
	vec4 ixy1 = permute(ixy + iz1);

	vec4 gx0 = ixy0 * (1.0 / 7.0);
	vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
	gx0 = fract(gx0);
	vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
	vec4 sz0 = step(gz0, vec4(0.0));
	gx0 -= sz0 * (step(0.0, gx0) - 0.5);
	gy0 -= sz0 * (step(0.0, gy0) - 0.5);

	vec4 gx1 = ixy1 * (1.0 / 7.0);
	vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
	gx1 = fract(gx1);
	vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
	vec4 sz1 = step(gz1, vec4(0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
	vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
	vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
	vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
	vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
	vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
	vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
	vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

	vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	g000 *= norm0.x;
	g010 *= norm0.y;
	g100 *= norm0.z;
	g110 *= norm0.w;
	vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	g001 *= norm1.x;
	g011 *= norm1.y;
	g101 *= norm1.z;
	g111 *= norm1.w;

	float n000 = dot(g000, Pf0);
	float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
	float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
	float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
	float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
	float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
	float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
	float n111 = dot(g111, Pf1);

	vec3 fade_xyz = fade(Pf0);
	vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
	vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
	float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
	return 2.2 * n_xyz;
}

float turbulence( vec3 p ) {
	float w = 100.0;
	float t = -.5;
	for (float f = 1.0 ; f <= 10.0 ; f++ ){
		float power = pow( 2.0, f );
		t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
	}
	return t;
}

float getBumpmap(vec3 pos){
		float noise = 1.0 * turbulence( .6 * vec3(1.0) + time/100.0 );

		float speed = 3.0f;
	float b = 0.4 * pnoise( 0.05 * pos + vec3(speed* time/3.0 ), vec3( 100.0 ) );
	float c = 0.5 * pnoise( 0.3 * pos + vec3( speed*time/5.0), vec3( 100.0 ) );

	float d = 0.1 * pnoise( 0.8 * pos+ vec3( speed*time/2.0 ), vec3( 100.0 ) );
	float e = 0.1 * pnoise( 3.9 * pos+ vec3( speed*time/5.0 ), vec3( 100.0 ) );
	return - 0.4*(noise + b + c + b*d  + e + max(c*b,0.0));
}

vec3 calculateNormal(vec3 pos){

	float dx = 0.5;

	vec3 A = vec3( 0.0, getBumpmap(vec3(pos.x, pos.y, pos.z)), 0.0);
	vec3 B = vec3( dx, getBumpmap(vec3(pos.x + dx , pos.y, pos.z)), 0.0);
	vec3 x = B - A;

	vec3 C = vec3( 0.0, getBumpmap(vec3(pos.x, pos.y, pos.z + dx)), dx);
	vec3 z = B - C;
	vec3 n = cross(x, z);

	return n;
}


void main()
{

	vec3 normal = normalize(calculateNormal(newPos));
	vec3 lightDir = normalize(vec3(0.0, 0.2, 0.5));


	// Colors 
	vec3 ambient_color = vec3(0.0);
	vec3 diffuse_color = vec3(0.2, 0.65, 0.8);
	vec3 specular_color = vec3(1.0);

	ambient_color = diffuse_color;

	// Diffuse
	float kd = 0.0f;
    vec3 diffuse = kd * clamp( dot( normal, lightDir ), 0.0, 1.0) * diffuse_color;

	// Ambient
	float ka = 0.0;
	vec3 ambient = ambient_color * ka;

	// Specular
	float ks = 0.1;
	vec3 V = normalize(newPos - camPos);
	vec3 R = 2.0*dot(lightDir,normal)*normal - lightDir;
	vec3 specular = ks * pow( clamp(dot(R, V), 0.0, 1.0), 14.9) * clear_color;	
		
	vec3 fog = clear_color * clamp( length(newPos - camPos)/10.0, 0.0, 1.0);
	vec3 phong = ambient + diffuse + specular;
	float height = clamp(0.03*(newPos.y + 2.0), 0.0, 1.0);

	vec2 screen_coord = vec2(gl_FragCoord.x / window_dim.x, gl_FragCoord.y / window_dim.y );
	
	// Depth
	vec3 depth_color = vec3(0.25, 0.4, 0.35);
	vec3 d1 =  0.001 * vec3(gl_FragCoord.z / gl_FragCoord.w);
	vec3 d2 = vec3(texture(depthTexture, screen_coord  + 0.02f*vec2(normal.x, normal.z)));
	vec3 depth = depth_color * clamp(10.0 * (d2.x-d1.x), 0.0, 0.8);

	// Refraction
	vec3 refraction_color = vec3(texture(refractionTexture, screen_coord + 0.04f*vec2(normal.x, normal.z) ));
	vec3 refraction = 0.3 *  ( refraction_color + 1.2*depth ) * pow(dot(normal, V) * 0.2 + 0.8, 0.02) + 0.2 * refraction_color;
	
	// Reflection
	vec3 reflection_color = 0.6*vec3(texture(reflectionTexture, screen_coord + 0.05f*vec2(normal.x, normal.z) ));
	vec3 reflection = (pow((1.0f - dot(normal, -V)), 2.9)*0.7 + 0.3) * reflection_color;

	vec3 color = phong + refraction + reflection;

	outputF = vec4(color, 1.0);

} 