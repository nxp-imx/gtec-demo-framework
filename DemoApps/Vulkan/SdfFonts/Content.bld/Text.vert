#version 310 es
precision mediump float;

layout(push_constant) uniform LayerBlock
{
  uniform mat4 ModelViewProj;
  // 0.25f / (spread * scale)
  uniform float Smoothing;
  // Between 0 and spread / textureSize
  uniform float ShadowOffsetX;
  uniform float ShadowOffsetY;
  // Between 0 and 0.5
  uniform float ShadowSmoothing;
  // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
  uniform float OutlineDistance;
}
g_pushConstant;

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 VertexTextureCoord;

layout(location = 0) out vec2 v_fragTextureCoord;

void main()
{
  gl_Position = g_pushConstant.ModelViewProj * VertexPosition;
  v_fragTextureCoord = VertexTextureCoord;
}
