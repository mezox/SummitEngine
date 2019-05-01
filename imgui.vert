#version 450

layout (push_constant) uniform PushConstants
{
	vec2 scale;
	vec2 translate;
} guiSettings;

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;

layout (location = 0) out vec2 outTexCoords;
layout (location = 1) out vec4 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	outTexCoords = texCoords;
	outColor = color;
	gl_Position = vec4(position * guiSettings.scale + guiSettings.translate, 0.0, 1.0);
}