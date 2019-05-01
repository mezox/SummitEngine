#version 450
 
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
 
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;
 
layout(location = 0) out vec2 outTexCoords;
 
void main()
{
   gl_Position = vec4(inPosition, 0.0, 1.0);
   outTexCoords = inTexCoords;
}