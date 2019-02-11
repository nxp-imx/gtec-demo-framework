#version 300 es
precision lowp float;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 TexCoord;

uniform float InstanceDistance;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
uniform float MaxHairLength;
uniform vec3 Displacement;

out mediump vec3 v_normal;
out mediump vec2 v_texcoord;
out mediump float v_layerDepth;

void main()
{
  float layerDepth = (float(gl_InstanceID) * InstanceDistance);

  vec4 pos = vec4(VertexPosition + (VertexNormal * MaxHairLength * layerDepth), 1.0);
  vec4 worldPosition = World * pos;
  // make the displacement non linear, to make it look more like fur
  float displacementFactor = pow(layerDepth, 3.0);
  // apply the displacement
  worldPosition.xyz += Displacement * displacementFactor;
  vec4 viewPosition = View * worldPosition;
  gl_Position = Projection * viewPosition;
  v_texcoord = TexCoord;
  v_normal = (World * vec4(VertexNormal, 1.0)).xyz;
  v_layerDepth = layerDepth;
}
