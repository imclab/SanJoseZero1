varying vec4 pos;

void main()
{
	gl_Position = ftransform();
	pos = gl_Position;
}