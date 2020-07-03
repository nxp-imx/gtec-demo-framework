#version 450 core

layout (set = 0, binding = 0) uniform UBO 
{
  mat4 ModelViewProj;
} ubo;

layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec4 inVertexColor;
layout (location = 2) in vec2 inVertexTextureCoord;

layout (location = 0) out vec4 outFragColor;
layout (location = 1) out vec2 outFragTextureCoord;

out gl_PerVertex 
{
  vec4 gl_Position;   
};

void main(void)
{
  gl_Position = ubo.ModelViewProj * vec4(inVertexPosition, 1.0);
  //gl_Position = vec4(inVertexPosition, 1.0);
  outFragColor = inVertexColor;
  outFragTextureCoord = inVertexTextureCoord;
}
