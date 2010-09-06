uniform sampler2DRect Tex0, Tex1;
//
//uniform float shine;
//uniform float fade;
//
const float blurSize = 5.0;
//const vec3 white = vec3(1.0);
//
void main (void)
{
	vec4 image = texture2DRect(Tex0, gl_TexCoord[0].st);
	vec4 composite = texture2DRect(Tex1, gl_TexCoord[1].st);
	gl_FragData[0] = vec4(image.rgb*composite.r, 1.0);
//	
//	vec4 sum = vec4(0.0);
	// take nine samples, with the distance blurSize between them
/*	
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t- 4.0*blurSize)) * 0.05;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t- 3.0*blurSize)) * 0.09;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t- 2.0*blurSize)) * 0.12;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t- blurSize)) * 0.15;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1])) * 0.16;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t+ blurSize)) * 0.15;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t+ 2.0*blurSize)) * 0.12;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t+ 3.0*blurSize)) * 0.09;
	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t+ 4.0*blurSize)) * 0.05;
 */
//	float blurAmount = blurSize;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s - 4.0*blurAmount, gl_TexCoord[1].t)) * 0.05;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s- 3.0*blurAmount, gl_TexCoord[1].t)) * 0.09;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s- 2.0*blurAmount, gl_TexCoord[1].t)) * 0.12;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s - blurAmount, gl_TexCoord[1].t)) * 0.15;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s, gl_TexCoord[1].t)) * 0.16;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s + blurAmount, gl_TexCoord[1].t)) * 0.15;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s+ 2.0*blurAmount, gl_TexCoord[1].t)) * 0.12;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s+ 3.0*blurAmount, gl_TexCoord[1].t)) * 0.09;
//	sum += texture2DRect(Tex1, vec2(gl_TexCoord[1].s+ 4.0*blurAmount, gl_TexCoord[1].t)) * 0.05;
	
	
//	vec3 color = image.rgb * sum.r;
	
//	vec3 blend = vec3(shine, shine, shine) * min(sum.r, .85) ;
//	color = color / (white - blend);
//
//	gl_FragData[0] = vec4(color, sum.a); 
//	gl_FragData[0] = sum;
}
