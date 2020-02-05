#version 310 es
precision lowp float;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TexCoord;

layout(std140, binding = 0) uniform UBO
{
  mat4 World;
  mat4 View;
  mat4 Projection;
  vec4 Displacement;
  vec4 AmbientColor;
  float MaxHairLength;
  float InstanceMul;
  float Reserved0;
  float Reserved1;
  vec4 LightDirection1;
  vec4 LightColor1;
}
g_ubo;

layout(location = 0) out mediump vec3 v_normal;
layout(location = 1) out mediump vec2 v_texcoord;

void main()
{
  vec4 pos = vec4(VertexPosition, 1.0);
  vec4 worldPosition = g_ubo.World * pos;
  vec4 viewPosition = g_ubo.View * worldPosition;

  gl_Position = g_ubo.Projection * viewPosition;
  v_texcoord = TexCoord;
  v_normal = normalize(g_ubo.World * vec4(VertexNormal, 1.0)).xyz;
}
