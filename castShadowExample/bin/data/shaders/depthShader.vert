varying vec3 normal, lightDir;
varying float dist;

uniform vec3 lightPos;

void main()
{	
	lightDir = vec3(gl_ProjectionMatrix * gl_ModelViewMatrix *  vec4(lightPos, 1.0));
	lightDir = normalize(-lightDir);
	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	dist = length(gl_Vertex.xyz)*0.1;
}

/*varying vec4 pos;

void main()
{
	gl_Position = ftransform();
	pos = gl_Position;
}

*/