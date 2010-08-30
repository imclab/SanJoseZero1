
varying vec3 norm;
varying vec3 ePos;
varying vec3 lightDir;
varying float dist;

void main()
{
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	
	ePos = ecPosition.xyz/ecPosition.w;
	norm = normalize(gl_NormalMatrix * gl_Normal);
	//norm.z = abs(norm.z);
	
	vec3 lightPos = vec3(0., 1.0, 0.0);//whatever
	lightDir = normalize(normalize(ePos) - lightPos);
	
	vec3 i = normalize(ePos);
	ePos = i;	
		
	gl_Position = ftransform();
	dist = pow(length(gl_Position.xyz/gl_Position.w), 2.0);
}

