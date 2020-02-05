#version 310 es
precision highp float;

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
layout(location = 2) out mediump float v_instanceId;
layout(location = 3) out mediump float v_dist;
layout(location = 4) out mediump float v_shadow;

void main()
{
  float instanceId = float(gl_InstanceIndex) * g_ubo.InstanceMul;

  vec4 pos = vec4(VertexPosition + (VertexNormal * g_ubo.MaxHairLength * instanceId), 1.0);
  vec4 worldPosition = g_ubo.World * pos;

  // make the displacement non linear, to make it look more like fur
  float displacementFactor = pow(instanceId, 3.0);

  // apply the displacement
  worldPosition.xyz += g_ubo.Displacement.xyz * displacementFactor;
  vec4 viewPosition = g_ubo.View * worldPosition;

  gl_Position = g_ubo.Projection * viewPosition;
  v_texcoord = TexCoord;
  v_normal = normalize(g_ubo.World * vec4(VertexNormal, 1.0)).xyz;
  v_instanceId = instanceId;

  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.
  v_dist = mix(1.0, 0.2, instanceId);
  v_shadow = mix(0.4, 1.0, instanceId);
}
