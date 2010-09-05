uniform vec3 lPos;
uniform mat4 invMat;

varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 lightDir;


mat4 GetAffineInverse(mat4 inMat)
{
/*
float entries[16];
entries[0] = inMat[0][0];	entries[1] = inMat[0][1];	entries[2] = inMat[0][2];	entries[3] = inMat[0][3];
entries[4] = inMat[1][0];	entries[5] = inMat[1][1];	entries[6] = inMat[1][2];	entries[7] = inMat[1][3];
entries[8] = inMat[2][0];	entries[9] = inMat[2][1];	entries[10] = inMat[2][2];	entries[11] = inMat[2][3];
entries[12] = inMat[3][0];	entries[13] = inMat[3][1];	entries[14] = inMat[3][2];	entries[15] = inMat[3][3];
*/
	mat4 inverMatr = mat4(inMat[0][0], inMat[1][0], inMat[2][0], 0.0,					   
						  inMat[0][1], inMat[1][1], inMat[2][1], 0.0,
						  inMat[0][2], inMat[1][2], inMat[2][2], 0.0,
						  -(inMat[0][0] * inMat[3][0] + inMat[0][1] * inMat[3][1] + inMat[0][2] * inMat[3][2]),
						  -(inMat[1][0] * inMat[3][0] + inMat[1][1] * inMat[3][1] + inMat[1][2] * inMat[3][2]),
						  -(inMat[2][0] * inMat[3][0] + inMat[2][1] * inMat[3][1] + inMat[2][2] * inMat[3][2]),
						  1.0);
	
	return inverMatr;
}
void main()
{		  
	gl_FrontColor = gl_Color;	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = gl_NormalMatrix * normalize(lPos - gl_Vertex.xyz);
	
	//get the modelViewMatrix
	mat4 modelMatrix = invMat * gl_ModelViewMatrix;
	vec4 ws	= modelMatrix * gl_Vertex;
		
	//transform to light's modelviewwmatrix	
	ShadowCoord = gl_TextureMatrix[7] * ws;	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex; //gl_TextureMatrix[7] * gl_Position;
	
}
