#version 430
//Triangles
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec3 normal;
out vec3 in_color;
out vec3 pos;
out vec3 sphere_center;

uniform mat4 MV;
uniform mat4 P;
uniform float pSize;

const vec3 RED = vec3(1.0, 0.0, 0.0);
const vec3 GREEN = vec3(0.0, 1.0, 0.0);
const vec3 BLUE = vec3(0.0, 0.0, 1.0);
const vec3 GREY = vec3(0.5, 0.5, 0.5);
const float PI = 3.1415926;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void make_face(vec3 a, vec3 b, vec3 c, vec3 face_color) {
    vec3 face_normal = normalize(cross(b - a, c - a));

    normal = face_normal;
    in_color = face_color;
	pos = b;
    gl_Position = P * MV * vec4(a, 1.0);
    EmitVertex();

    normal = face_normal;
    in_color = face_color;
    gl_Position = P * MV * vec4(b, 1.0);
    EmitVertex();

    normal = face_normal;
    in_color = face_color;
    gl_Position = P * MV * vec4(c, 1.0);
    EmitVertex();
}

void make_sphere(vec3 center, float radius, int ySeg, int xSeg) {


	float y_stepSize = PI / ySeg; 
	float x_stepSize = 2.0 * PI / xSeg; 

	// Top piece
	vec3 top = radius * vec3(0,1,0) - center;
	for(int j = 0; j < xSeg; j++){
		
		vec4 stepper = vec4(0,1,0,1);
		stepper *= rotationMatrix(vec3(1,0,0), y_stepSize);
		
		float y_offset = dot(stepper.xyz, vec3(0,1,0));
		float xz_offset = sqrt( 1 - pow(y_offset, 2.0) );
		
		vec4 a_raw = vec4(vec3(xz_offset, y_offset, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * j);
		vec4 b_raw = vec4(vec3(xz_offset, y_offset, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * (j+1));

		vec3 a = radius * (a_raw.xyz) - center;
		vec3 b = radius * (b_raw.xyz) - center;

		make_face(top, b, a, RED);
	}

	// Middle piece
	for(int i = 1; i < ySeg-1; i++){

		for(int j = 0; j < xSeg; j++){


			vec4 stepper = vec4(0,1,0,1);
			stepper *= rotationMatrix(vec3(1,0,0), y_stepSize*i);

			float y_offset1 = dot(stepper.xyz, vec3(0,1,0));
			float xz_offset1 = sqrt( 1 - pow(y_offset1, 2.0) );

			stepper *= rotationMatrix(vec3(1,0,0), y_stepSize);

			float y_offset2 = dot(stepper.xyz, vec3(0,1,0));
			float xz_offset2 = sqrt( 1 - pow(y_offset2, 2.0) );


			vec4 a_raw = vec4(vec3(xz_offset1, y_offset1, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * j);
			vec4 b_raw = vec4(vec3(xz_offset1, y_offset1, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * (j+1));
				
			vec4 c_raw = vec4(vec3(xz_offset2, y_offset2, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * j);
			vec4 d_raw = vec4(vec3(xz_offset2, y_offset2, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * (j+1));

			vec3 a = radius * (a_raw.xyz) - center;
			vec3 b = radius * (b_raw.xyz) - center;
			vec3 c = radius * (c_raw.xyz) - center;
			vec3 d = radius * (d_raw.xyz) - center;

			make_face(a,d,c, RED);
			make_face(d,a,b, RED);
		}
	}

	// Bottom piece
	vec3 bottom = radius * vec3(0,-1,0) - center;
	for(int j = 0; j < xSeg; j++){
		
		vec4 stepper = vec4(0,1,0,1);
		stepper *= rotationMatrix(vec3(1,0,0), y_stepSize);
		
		float y_offset = dot(stepper.xyz, vec3(0,-1,0));
		float xz_offset = sqrt( 1 - pow(y_offset, 2.0) );

		vec4 a_raw = vec4(vec3(xz_offset, y_offset, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * j);
		vec4 b_raw = vec4(vec3(xz_offset, y_offset, 0), 1.0) * rotationMatrix(vec3(0,1,0), x_stepSize * (j+1));

		vec3 a = radius * (a_raw.xyz) - center;
		vec3 b = radius * (b_raw.xyz) - center;
		make_face(bottom, a, b, RED);
	}


    EndPrimitive();
}

void main() {
	sphere_center = gl_in[0].gl_Position.xyz;
    make_sphere(sphere_center, pSize, 3, 3);
}