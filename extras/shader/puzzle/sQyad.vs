#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 Tex;
out float bloom;
out float fin;

uniform int indexX;
uniform int indexY;
uniform float aBloom;
uniform mat4 view;
uniform mat4 projection;
uniform float finish;

vec2 TexCord[6] = vec2[6](
	vec2(0.205f,0.21f),
	vec2(0.205f,-0.01f),
	vec2(-0.005f,-0.01f),
	vec2(-0.005f,-0.01f),
	vec2(-0.005f,0.21f),
	vec2(0.205f,0.21f)
); 

void main()
{
	gl_Position = projection * view * vec4(aPos,0.0f,1.0f);
	bloom = aBloom;
	if(bloom>0.5f)
		Tex = TexCord[gl_VertexID];
	else
		Tex = aTex;
		fin = finish;
}