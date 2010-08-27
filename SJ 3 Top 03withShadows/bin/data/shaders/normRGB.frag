varying vec3 norm;
varying vec3 ePos;
varying vec3 lightDir;
varying float dist;


uniform sampler2D inTex;

void main()
{	
	vec3 ref;
	vec4 reflectColor;
	vec2 Rref;
	vec2 Gref;
	vec2 Bref;
	float trans;
	vec3 normal = norm;
	
    float intensity = dot(normalize(ePos), normalize(norm));
    float lightIntensity = dot(normalize(lightDir), normalize(norm));
	
	if(intensity >= 0.0){
		normal = vec3(1.0) - normal;
	}
	Rref = refract(ePos, normal, .63).xy;
	Gref = refract((ePos), normal, .645).xy;
	Bref = refract(normalize(ePos), normal, .66).xy;
	
	reflectColor.x = texture2D(inTex, Rref*.5+.5).x;
	reflectColor.y = texture2D(inTex, Gref*.5+.5).y;
	reflectColor.z = texture2D(inTex, Bref*.5+.5).z;
	reflectColor.w = 1.;
	
	intensity = pow(abs(intensity), 1.5)* .9;
	lightIntensity = pow(abs(lightIntensity), 50.0)*0.5;
	vec4 color = reflectColor * (1.0-intensity) + vec4(.1)*(intensity);
	color.xyz += vec3(1.)*(lightIntensity);
	
	gl_FragColor = vec4(color.xyz*dist, color.w);
} 