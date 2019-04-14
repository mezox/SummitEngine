#version 450

layout(binding = 0) uniform UniformBufferObject {
	vec2 scale;
	vec2 translate;
} ubo;

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
	gl_Position = vec4(position * vec2(2.0/1280.0, 2.0/720.0) + vec2(-1.0, -1.0), 0.0, 1.0);
}