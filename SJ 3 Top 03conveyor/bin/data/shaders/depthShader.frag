varying vec4 pos;

void main()
{	
	float depthVal = pow(pos.z/pos.w, 1.0);
	gl_FragColor =vec4(vec3(depthVal), 1.0);
}