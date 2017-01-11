#version 430 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;


void main()
{

	float blur = pow( gl_FragCoord.z / gl_FragCoord.w /25.0, 2.0) - 0.5;
	float offset = 1.0 /  120;  

    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f,    offset),  // top-center
        vec2(offset,  offset),  // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offset,  0.0f),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f,    -offset), // bottom-center
        vec2(offset,  -offset)  // bottom-right    
    );

    
float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);
//float kernel[9] = float[](
//        -1, -1, -1,
//        -1,  9, -1,
//        -1, -1, -1
//    );
  //  float kernel[9] = float[](
  //      1, 1, 1,
		//1, -8, 1, 
		//1, 1, 1
  //  );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    color = vec4(col, 1.0);
	//color = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);

	//vec4 top         = texture(screenTexture, vec2(TexCoords.x, TexCoords.y + 1.0 / 200.0));
	//vec4 bottom      = texture(screenTexture, vec2(TexCoords.x, TexCoords.y - 1.0 / 200.0));
	//vec4 left        = texture(screenTexture, vec2(TexCoords.x - 1.0 / 300.0, TexCoords.y));
	//vec4 right       = texture(screenTexture, vec2(TexCoords.x + 1.0 / 300.0, TexCoords.y));
	//vec4 topLeft     = texture(screenTexture, vec2(TexCoords.x - 1.0 / 300.0, TexCoords.y + 1.0 / 200.0));
	//vec4 topRight    = texture(screenTexture, vec2(TexCoords.x + 1.0 / 300.0, TexCoords.y + 1.0 / 200.0));
	//vec4 bottomLeft  = texture(screenTexture, vec2(TexCoords.x - 1.0 / 300.0, TexCoords.y - 1.0 / 200.0));
	//vec4 bottomRight = texture(screenTexture, vec2(TexCoords.x + 1.0 / 300.0, TexCoords.y - 1.0 / 200.0));
	//vec4 sx = -topLeft - 2 * left - bottomLeft + topRight   + 2 * right  + bottomRight;
	//vec4 sy = -topLeft - 2 * top  - topRight   + bottomLeft + 2 * bottom + bottomRight;
	//vec4 sobel = sqrt(sx * sx + sy * sy);
	//color = sobel;
}  