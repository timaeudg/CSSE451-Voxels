//#version 130

uniform mat4 M;
uniform mat4 P;
uniform mat4 C;

uniform float time;

attribute vec3 pos;
attribute vec3 color;
attribute vec3 norm;

varying float d;
varying float s;

vec4 lightPos = vec4(0, 0, 0, 1);

varying vec4 smoothColor;
varying vec4 surfaceNormal;
varying vec4 lightVector;
varying vec4 viewVector;

mat4 rotateY(float a){

	return mat4(1, 0, 0, 0,
		        0, 1, 0, 0,
				0, 0, 1, 0,
				4, 8, 4, 0)*mat4(
			cos(a), sin(a), 0, 0,
			-sin(a), cos(a), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

}

void main()
{	
	vec4 p = C*M*vec4(pos, 1);
	mat4 lightR = rotateY(time*0.6);
	vec4 lPos = C*lightPos;
	vec4 unNormalized = C*M*vec4(norm,0);
	surfaceNormal = normalize(unNormalized);
	lightVector = normalize(lPos - p);
	viewVector = normalize(-p);
	smoothColor = vec4(color, 1);

	gl_Position = P*p;
}
