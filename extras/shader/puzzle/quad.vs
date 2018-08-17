#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 Tex;

uniform int indexX;
uniform int indexY;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(aPos,0.0f,1.0f);
	Tex = aTex;
	float dist = 0.2f;
	Tex.x += (indexX*dist);
	Tex.y += (indexY*dist);
}