varying vec3 normal, eyeVec;
#define MAX_LIGHTS 8
#define NUM_LIGHTS 3
varying vec3 lightDir[MAX_LIGHTS];
varying vec4 ShadowCoord;

void main()
{
	gl_Position = ftransform();
	normal = gl_NormalMatrix * gl_Normal;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	ShadowCoord = gl_TextureMatrix[7] * gl_Vertex;
	
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	eyeVec = -vVertex.xyz;
	int i;
	for (i=0; i<NUM_LIGHTS; ++i)
    lightDir[i] =
	vec3(gl_LightSource[i].position.xyz - vVertex.xyz);
}
/*
varying vec3 norm;
varying vec3 ePos;

void main()
{	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	norm = normalize(gl_NormalMatrix * gl_Normal);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
*/