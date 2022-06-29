#version 450 core

layout(push_constant) uniform LayerBlock
{
  mat4 ModelViewProj;
}
g_pushConstant;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec4 inVertexColor;
layout(location = 2) in vec2 inVertexTextureCoord;

layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec2 outFragTextureCoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main(void)
{
  gl_Position = g_pushConstant.ModelViewProj * vec4(inVertexPosition, 1.0);
  outFragColor = inVertexColor;
  outFragTextureCoord = inVertexTextureCoord;
}
