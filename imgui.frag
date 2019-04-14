#version 450

layout (binding = 1) uniform sampler2D fontSampler;

layout (location = 0) in vec2 texCoords;
layout (location = 1) in vec4 color;

layout (location = 0) out vec4 outColor;

void main() 
{
	outColor = vec4(color.rgb, texture(fontSampler, texCoords).r);
}