#version 430 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D Texture1;

in vec2 Tex;

vec3 gammaCorrect(sampler2D Texture1,vec2 Tex)
{
	vec3 DiffColo = texture(Texture1,Tex).rgb;
	float gamma  = 1/2.4f;
	float a = 0.055f;
	float lim = 0.0031308;
	if(DiffColo.r<=lim)
		DiffColo.r *= 12.92;
	else
		DiffColo.r = pow((DiffColo.r),gamma)*(1+a)-a;
	if(DiffColo.g<=lim)
		DiffColo.g *= 12.92;
	else
		DiffColo.g = pow((DiffColo.g),gamma)*(1+a)-a;
	if(DiffColo.b<=lim)
		DiffColo.b *= 12.92;
	else
		DiffColo.b = pow((DiffColo.b),gamma)*(1+a)-a;

	return DiffColo;
}

void main()
{
	vec3 Color = texture(Texture1,Tex).rgb;
	FragColor = vec4(Color,1.0f);
}