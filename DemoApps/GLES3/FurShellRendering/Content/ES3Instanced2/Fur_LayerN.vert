#version 300 es
precision lowp float;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 TexCoord;

uniform float InstanceMul;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
uniform float MaxHairLength;
uniform vec3 Displacement;

out mediump vec3 v_normal;
out mediump vec2 v_texcoord;
out mediump float v_instanceId;
out mediump float v_dist;
out mediump float v_shadow;

void main()
{
  float instanceId = InstanceMul + (float(gl_InstanceID) * InstanceMul);

  vec4 pos = vec4(VertexPosition + (VertexNormal * MaxHairLength * instanceId), 1.0);
  vec4 worldPosition = World * pos;

  // make the displacement non linear, to make it look more like fur
  float displacementFactor = pow(instanceId, 3.0);

  // apply the displacement
  worldPosition.xyz += Displacement * displacementFactor;
  vec4 viewPosition = View * worldPosition;

  gl_Position = Projection * viewPosition;
  v_texcoord = TexCoord;
  v_normal = normalize(World * vec4(VertexNormal, 1.0)).xyz;
  v_instanceId = instanceId;

  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.
  v_dist = mix(1.0, 0.2, instanceId);
  v_shadow = mix(0.4, 1.0, instanceId);
}
