//const float Eta = .66;		//ratio of indices of refraction
//uniform float eta;
//varying vec3 Reflect;
varying vec3 norm;
varying vec3 ePos;

void main()
{
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	norm = normalize(gl_NormalMatrix * gl_Normal);
	
	gl_Position = ftransform();
}

